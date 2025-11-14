#pragma once

#include <cmath>
#include <ostream>

struct Position {
  double x;
  double y;

  Position operator+(const Position &pos) {
    Position result({x + pos.x, y + pos.y});
    return result;
  }

  Position operator-(const Position &pos) {
    Position result({x - pos.x, y - pos.y});
    return result;
  }

  Position mod(const double a) {
    Position result({std::fmod(x, a), std::fmod(y, a)});
    return result;
  }
};

inline Position operator*(double a, const Position &pos) {
  Position result({a * pos.x, a * pos.y});
  return result;
}

inline Position operator*(const Position &pos, double a) {
  Position result({a * pos.x, a * pos.y});
  return result;
}

inline double norm(const Position &pos) {
  return (pos.x * pos.x) + (pos.y * pos.y);
}

inline double angle(const Position &pos1, const Position &pos2) {
  return std::atan2(pos1.y - pos2.y, pos1.x - pos2.x);
}

std::ostream &operator<<(std::ostream &in, const Position &pos) {
  in << "(" << pos.x << "," << pos.y << ")";
  return in;
}
