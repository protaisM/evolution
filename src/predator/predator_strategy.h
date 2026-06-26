#pragma once

#include "map.h"
#include "position.h"

class PredatorStrategy {
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

  virtual ~PredatorStrategy() {};

protected:
  Map *m_map;

public:
  PredatorStrategy(Map *map) : m_map(map) {}
};

struct Path {
  double frequency;
  Position start_point;
  Position end_point;

  Position operator()(double t) const {
    double T = (cos(frequency * t) + 1) / 2;
    return T * start_point + (1 - T) * end_point;
  }
};

class FollowPath : public PredatorStrategy {
  // TODO: for the moment the angle is wrong
private:
  Path m_path;

public:
  FollowPath(Map *map) : PredatorStrategy(map) {
    m_path.start_point = m_map->rnd_position();
    m_path.end_point = m_map->rnd_position();
    m_path.frequency = 5 * (double)std::rand() / RAND_MAX;
  }

  FollowPath(Map *map, Position start_point, Position end_point,
             double frequency)
      : PredatorStrategy(map) {
    m_path.start_point = start_point;
    m_path.end_point = end_point;
    m_path.frequency = frequency;
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

class Static : public PredatorStrategy {
private:
  double m_time_threshold;
  bool m_randomize_position;

public:
  Static(Map *map, double time_threshold, bool random_pos = false)
      : PredatorStrategy(map), m_time_threshold(time_threshold),
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

class RandomWalk : public PredatorStrategy {

protected:
  double m_velocity;
  bool m_randomize_position;
  double m_time_threshold;

public:
  RandomWalk(Map *map, double velocity, bool random_pos, double time_threshold)
      : PredatorStrategy(map), m_velocity(velocity), m_randomize_position(random_pos),
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
