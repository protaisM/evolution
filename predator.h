#pragma once

#include "map.h"
#include "position.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace Predator {

inline double rand_angle() {
  return 2 * M_PI * ((double)rand() / (double)RAND_MAX);
}

class Shape {
  // ------------------------------------------------------------------------ //
  // virtual functions, every subclass should implement these
public:
  virtual bool is_in(Position pos_predator, Position pos) const = 0;
  virtual void draw(sf::RenderWindow *window, Position offset,
                    double window_size) const = 0;
  // ------------------------------------------------------------------------ //

  virtual ~Shape() {};

protected:
  Map *m_map;

public:
  Shape(Map *map) : m_map(map) {}
};

class Strategy {
  // ------------------------------------------------------------------------ //
  // virtual functions, every subclass should implement these
public:
  virtual PositionAngle
  start_of_the_round(PositionAngle previous_state) const = 0;
  virtual PositionAngle advance(PositionAngle state, // the previous state
                                double time,         // the current time
                                double dt) = 0;      // the increment
  virtual bool killing_strategy(double time) const = 0;
  // ------------------------------------------------------------------------ //

  virtual ~Strategy() {};

protected:
  Map *m_map;

public:
  Strategy(Map *map) : m_map(map) {}
};

class Predator {

private:
  PositionAngle m_state;
  double m_internal_clock;

  Shape *m_shape;
  Strategy *m_strategy;

public:
  Predator(Shape *shape, Strategy *strategy)
      : m_shape(shape), m_strategy(strategy) {
    start_of_the_round();
  }

  bool is_in_predator(Position pos) const {
    if (m_shape->is_in(m_state.position, pos)) {
      return m_strategy->killing_strategy(m_internal_clock);
    }
    return false;
  }

  void draw(sf::RenderWindow *window, sf::Vector2f offset,
            double window_size) const {
    Position position_offset({offset.x, offset.y});
    m_shape->draw(window, position_offset + m_state.position * window_size,
                  window_size);
  }

  void start_of_the_round() {
    m_internal_clock = 0.0;
    PositionAngle new_state = m_strategy->start_of_the_round(m_state);
    m_state = new_state;
  }

  void do_one_step(double dt) {
    m_internal_clock += dt;
    m_state = m_strategy->advance(m_state, m_internal_clock, dt);
  }

  Position get_position() const { return m_state.position; }
  PositionAngle get_state() const { return m_state; }
};

// ##########################################################################
// ################################# Shapes #################################
// ##########################################################################

class Circle : public Shape {
private:
  double m_radius;

public:
  Circle(Map *map, double radius = 0.1) : Shape(map), m_radius(radius) {}

  bool is_in(Position pos_predator, Position pos) const override {
    if (m_map->distance(pos, pos_predator) < m_radius) {
      return true;
    }
    return false;
  }

  void draw(sf::RenderWindow *window, Position offset,
            double window_size) const override {
    sf::Vector2f position({(float)offset.x, (float)offset.y});
    sf::CircleShape death(m_radius * window_size);
    death.setPosition(position);
    death.setFillColor(sf::Color(255, 0, 0, 127));
    death.setOrigin(m_radius * window_size, m_radius * window_size);
    window->draw(death);
  }
};

class Rectangle : public Shape {
private:
  double m_x_length;
  double m_y_length;

public:
  Rectangle(Map *map, double x_length, double y_length)
      : Shape(map), m_x_length(x_length), m_y_length(y_length) {}

  bool is_in(Position pos_predator, Position pos) const override {
    if (std::abs(pos_predator.x - pos.x) < m_x_length / 2 and
        std::abs(pos_predator.y - pos.y) < m_y_length / 2) {
      return true;
    }
    return false;
  }

  void draw(sf::RenderWindow *window, Position offset,
            double window_size) const override {
    sf::Vector2f position({(float)offset.x, (float)offset.y});
    sf::RectangleShape death;
    death.setSize({static_cast<float>(m_x_length * window_size),
                   static_cast<float>(m_y_length * window_size)});
    death.setPosition(position);
    death.setFillColor(sf::Color{255, 0, 0, 127});
    death.setOrigin(m_x_length * window_size / 2, m_y_length * window_size / 2);
    window->draw(death);
  }
};

// ##########################################################################
// ############################## Strategies ################################
// ##########################################################################

struct Path {
  double frequency;
  Position start_point;
  Position end_point;

  Position operator()(double t) const {
    double T = (cos(frequency * t) + 1) / 2;
    return T * start_point + (1 - T) * end_point;
  }
};

class FollowPath : public Strategy {
  // TODO: for the moment the angle is wrong
private:
  Path m_path;

public:
  FollowPath(Map *map) : Strategy(map) {
    m_path.start_point = m_map->rnd_position();
    m_path.end_point = m_map->rnd_position();
    m_path.frequency = 5 * (double)std::rand() / RAND_MAX;
  }

  PositionAngle
  start_of_the_round(PositionAngle /*previous_state*/) const override {
    return {m_path.start_point, 0};
  }

  PositionAngle advance(PositionAngle /*state*/, double time,
                        double /*dt*/) override {
    return {m_path(time), 0};
  }

  bool killing_strategy(double /*time*/) const override { return true; }
};

class Static : public Strategy {
private:
  double m_time_threshold;
  bool m_randomize_position;

public:
  Static(Map *map, double time_threshold, bool random_pos = false)
      : Strategy(map), m_time_threshold(time_threshold),
        m_randomize_position(random_pos) {}

  PositionAngle
  start_of_the_round(PositionAngle previous_state) const override {
    if (m_randomize_position) {
      return {m_map->rnd_position(), rand_angle()};
    }
    return previous_state;
  }

  bool killing_strategy(double time) const override {
    return time > m_time_threshold;
  };

  PositionAngle advance(PositionAngle state, double /*time*/,
                        double /*dt*/) override {
    return state;
  }
};

class RandomWalk : public Strategy {

protected:
  double m_velocity;
  bool m_randomize_position;
  double m_time_threshold;

public:
  RandomWalk(Map *map, double velocity, bool random_pos, double time_threshold)
      : Strategy(map), m_velocity(velocity), m_randomize_position(random_pos),
        m_time_threshold(time_threshold) {}

  PositionAngle
  start_of_the_round(PositionAngle previous_state) const override {
    if (m_randomize_position) {
      return {m_map->rnd_position(), rand_angle()};
    }
    return previous_state;
  }

  bool killing_strategy(double time) const override {
    return time > m_time_threshold;
  };
};

class Run_In_Circle : public RandomWalk {
public:
  Run_In_Circle(Map *map, double velocity = 0.2, bool random_pos = true,
                double time_threshold = 0.5)
      : RandomWalk(map, velocity, random_pos, time_threshold) {}

  PositionAngle advance(PositionAngle state, double /*time*/,
                        double dt) override {
    PositionAngle result = state;
    result.angle =
        M_PI / 2 + atan2((state.position.y - 0.5), (state.position.x - 0.5));
    if (m_map->distance(state.position, m_map->get_center()) >=
        m_map->get_radius()) {
      result.angle += dt * rand_angle();
    } else {
      result.angle += dt * (rand_angle() - M_PI);
    }
    Position direction({std::cos(state.angle), std::sin(state.angle)});
    Position pos = state.position + dt * m_velocity * direction;
    if (m_map->is_in(pos)) {
      result.position = pos;
    } else {
      result.position = m_map->project_on_map(pos);
    }
    return result;
  }
};

class Bounce : public RandomWalk {
public:
  Bounce(Map *map, double velocity = 0.2, bool random_pos = false,
         double time_threshold = 0.5)
      : RandomWalk(map, velocity, random_pos, time_threshold) {}

protected:
  PositionAngle advance(PositionAngle state, double /*time*/,
                        double dt) override {
    PositionAngle result = state;
    Position direction({std::cos(state.angle), std::sin(state.angle)});
    Position pos = state.position + dt * m_velocity * direction;
    if (m_map->is_in(pos)) {
      result.position = pos;
    } else {
      result.position = m_map->project_on_map(pos);
      result.angle = rand_angle();
    }
    return result;
  }
};

class Straigth : public RandomWalk {
public:
  Straigth(Map *map, double velocity = 0.2, bool random_pos = false,
           double time_threshold = 0.5)
      : RandomWalk(map, velocity, random_pos, time_threshold) {}

protected:
  PositionAngle advance(PositionAngle state, double /*time*/,
                        double dt) override {
    PositionAngle result = state;
    Position direction({std::cos(state.angle), std::sin(state.angle)});
    Position pos = state.position + dt * m_velocity * direction;
    if (m_map->is_in(pos)) {
      result.position = pos;
    } else {
      result.position = m_map->project_on_map(pos);
    }
    result.angle += dt * (rand_angle() - M_PI);
    return result;
  }
};
}; // namespace Predator
