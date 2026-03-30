#pragma once

#include "map.h"
#include "position.h"

class Food {
public:
  static unsigned int counter;

private:
  unsigned int m_id;

  Map *m_map;
  Position m_position;
  double m_radius;

public:
  Food(Map *map, Position position, double radius=0.01)
      : m_map(map), m_position(position), m_radius(radius) {
    if (radius <= 0) {
      throw std::runtime_error("Invalid food radius");
    }
    counter++;
    m_id = counter;
  }

  inline bool can_eat(Position position_animal) {
    return m_map->distance(position_animal, m_position) < m_radius;
  }
};
