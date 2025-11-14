#pragma once

// #include <SFML/Graphics.hpp>
#include <cmath>

#include "position.h"

class Map {
public:
  virtual bool is_in(Position) const = 0;
  virtual Position rnd_position() const = 0;
  virtual Position project_on_map(Position) const = 0;
  virtual double distance(Position, Position) const = 0;
  virtual Position get_center() const = 0;
};

class Square : public Map {
private:
  double m_side_length;

public:
  Square(double side_length) : m_side_length(side_length) {}

  virtual bool is_in(Position pos) const override {
    if (pos.x < 0 or pos.x > m_side_length or pos.y < 0 or
        pos.y > m_side_length) {
      return false;
    }
    return true;
  }

  virtual Position get_center() const override {
    Position center({m_side_length / 2, m_side_length / 2});
    return center;
  }

  virtual Position rnd_position() const override {
    Position result;
    result.x = ((double)rand() / ((double)RAND_MAX)) * m_side_length;
    result.y = ((double)rand() / ((double)RAND_MAX)) * m_side_length;
    return result;
  }

  virtual Position project_on_map(Position pos) const override {
    Position projection;
    projection.x = fmod(pos.x, 1.0);
    projection.y = fmod(pos.y, 1.0);
    return pos;
  };

  virtual double distance(Position pos1, Position pos2) const override {
    double dist_x = pos1.x - pos2.x;
    double dist_y = pos1.y - pos2.y;
    return std::sqrt((dist_x) * (dist_x) + (dist_y) * (dist_y));
  }
};

class Circle : public Map {
private:
  double m_diameter;

public:
  Circle(double diameter) : m_diameter(diameter) {}

  virtual bool is_in(Position pos) const override {
    if (distance(pos, get_center()) > m_diameter / 2) {
      return false;
    }
    return true;
  }

  virtual Position get_center() const override {
    Position center({m_diameter / 2, m_diameter / 2});
    return center;
  }

  virtual Position rnd_position() const override {
    Position result;
    do {
      result.x = ((double)rand() / ((double)RAND_MAX)) * m_diameter;
      result.y = ((double)rand() / ((double)RAND_MAX)) * m_diameter;
    } while (!is_in(result));
    return result;
  }

  virtual Position project_on_map(Position pos) const override {
    // TODO: needs position maths
  };

  virtual double distance(Position pos1, Position pos2) const override {
    double dist_x = pos1.x - pos2.x;
    double dist_y = pos1.y - pos2.y;
    return std::sqrt((dist_x) * (dist_x) + (dist_y) * (dist_y));
  }
};
