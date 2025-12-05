#pragma once

#include "map.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>

namespace Predator {
inline double rand_angle() {
  return 2 * M_PI * ((double)rand() / (double)RAND_MAX);
}

class BasePredator {
protected:
  bool m_randomize_position;
  Map *m_map;
  double m_velocity;

  double m_angle;
  Position m_position;

public:
  BasePredator(Map *map, double velocity, bool random_pos)
      : m_randomize_position(random_pos), m_map(map), m_velocity(velocity),
        m_position(map->rnd_position()) {}

  BasePredator() {}

  void clear_position() {
    if (m_randomize_position)
      m_position = m_map->rnd_position();
  }

  Position get_position() { return m_position; }

public:
  virtual bool is_in_death_zone(Position pos, double time) = 0;
  virtual void advance(double dt) = 0;
  virtual void draw(sf::RenderWindow *window, double window_size) const = 0;
};

class CircleShaped : public BasePredator {
protected:
  double m_radius;

public:
  CircleShaped(Map *map, double radius, double velocity, bool random_pos)
      : BasePredator(map, velocity, random_pos) {
    m_radius = radius;
  }

  CircleShaped() : BasePredator() {}

  bool is_in_death_zone(Position pos, double /*time*/) override {
    if (m_map->distance(pos, m_position) < m_radius) {
      return true;
    }
    return false;
  }

  void draw(sf::RenderWindow *window, double window_size) const override {
    sf::CircleShape death(m_radius * window_size);
    death.setPosition(m_position.x * window_size, m_position.y * window_size);
    death.setFillColor(sf::Color::Red);
    death.setOrigin(m_radius * window_size, m_radius * window_size);
    window->draw(death);
  }
};

class CircleShaped_RunInCircle : public CircleShaped {
public:
  CircleShaped_RunInCircle(Map *map, double radius, double velocity = 0.4,
                           bool random_pos = true)
      : CircleShaped(map, radius, velocity, random_pos) {
    m_radius = radius;
  }

  CircleShaped_RunInCircle() : CircleShaped() {}

  void advance(double dt) override {
    m_angle = M_PI / 2 + atan2((m_position.y - 0.5), (m_position.x - 0.5));
    if (m_map->distance(m_position, m_map->get_center()) >=
        m_map->get_radius() - m_radius) {
      m_angle += dt * rand_angle();
    } else {
      m_angle += dt * (rand_angle() - M_PI);
    }
    Position direction({std::cos(m_angle), std::sin(m_angle)});
    Position pos = m_position + dt * m_velocity * direction;
    if (m_map->is_in(pos)) {
      m_position = pos;
    } else {
      m_position = m_map->project_on_map(pos);
    }
  }
};

struct CircleShaped_Bounce : public CircleShaped {
public:
  CircleShaped_Bounce(Map *map, double radius, double velocity = 0.4,
                      bool random_pos = false)
      : CircleShaped(map, radius, velocity, random_pos) {
    m_radius = radius;
  }

  CircleShaped_Bounce() : CircleShaped() {}

  void advance(double dt) override {
    Position direction({std::cos(m_angle), std::sin(m_angle)});
    Position pos = m_position + dt * m_velocity * direction;
    if (m_map->is_in(pos)) {
      m_position = pos;
    } else {
      m_position = m_map->project_on_map(pos);
      m_angle = rand_angle();
    }
  }
};

struct CircleShaped_Straight : public CircleShaped {
public:
  CircleShaped_Straight(Map *map, double radius, double velocity = 0.4,
                        bool random_pos = false)
      : CircleShaped(map, radius, velocity, random_pos) {}

  CircleShaped_Straight() : CircleShaped() {}

  void advance(double dt) override {
    Position direction({std::cos(m_angle), std::sin(m_angle)});
    Position pos = m_position + dt * m_velocity * direction;
    if (m_map->is_in(pos)) {
      m_position = pos;
    } else {
      m_position = m_map->project_on_map(pos);
    }
    m_angle += dt * (rand_angle() - M_PI);
  }
};

class RectangleShaped : public BasePredator {
protected:
  double m_x_length;
  double m_y_length;
  double m_time_threshold;

public:
  RectangleShaped(Map *map, double x_length, double y_length,
                  double time_threshold, double velocity, bool random_pos)
      : BasePredator(map, velocity, random_pos), m_x_length(x_length),
        m_y_length(y_length), m_time_threshold(time_threshold) {}

  RectangleShaped() : BasePredator() {}

  bool is_in_death_zone(Position pos, double time) override {
    if (time <= m_time_threshold) {
      return false;
    }
    if (std::abs(m_position.x - pos.x) < m_x_length / 2 and
        std::abs(m_position.y - pos.y) < m_y_length / 2) {
      return true;
    }
    return false;
  }

  void draw(sf::RenderWindow *window, double window_size) const override {
    sf::RectangleShape death;
    death.setSize({static_cast<float>(m_x_length * window_size),
                   static_cast<float>(m_y_length * window_size)});
    death.setPosition(m_position.x * window_size, m_position.y * window_size);
    death.setFillColor(sf::Color{255, 0, 0, 127});
    death.setOrigin(m_x_length * window_size / 2, m_y_length * window_size / 2);
    window->draw(death);
  }
};

struct RectangleShaped_Static : public RectangleShaped {
public:
  RectangleShaped_Static(Map *map, double x_length, double y_length,
                         double time_threshold, bool random_pos = true)
      : RectangleShaped(map, x_length, y_length, time_threshold, 0,
                        random_pos) {}

  RectangleShaped_Static() : RectangleShaped() {}

  void advance(double /*dt*/) override {}
};

}; // namespace Predator
