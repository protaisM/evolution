#pragma once

// #include <SFML/Graphics.hpp>
#include <cmath>

struct Position {
  double x;
  double y;
};

class Map {
public:
  virtual bool is_in(Position) const = 0;
  virtual Position rnd_position() const = 0;
};

class Square : public Map {
private:
  double m_side_length;

public:
  Square(double side_length) : m_side_length(side_length) {}

  virtual bool is_in(Position pos) const {
    if (pos.x < 0 or pos.x > m_side_length or pos.y < 0 or
        pos.y > m_side_length) {
      return false;
    }
    return true;
  }

  virtual Position rnd_position() const {
    Position result;
    result.x = ((double)std::rand() / ((double)RAND_MAX)) * m_side_length;
    result.y = ((double)std::rand() / ((double)RAND_MAX)) * m_side_length;
    return result;
  }
};
