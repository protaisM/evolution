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
      if(pos.x>m_side_length){pos.x=m_side_length;}
      if(pos.y>m_side_length){pos.y=m_side_length;}
      if(pos.x<0){pos.x=0;}
      if(pos.y<0){pos.y=0;}
    return pos; //.mod(m_side_length);
  };

  virtual double distance(Position pos1, Position pos2) const override {
    return norm(pos1 - pos2);
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
    Position center = get_center();
    return (pos - center) * (m_diameter / (2 * norm(pos - center))) + center;
  };

  virtual double distance(Position pos1, Position pos2) const override {
    return norm(pos1 - pos2);
  }
};
