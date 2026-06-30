#pragma once

#include "map.h"
#include "position.h"
#include <SFML/System/Vector2.hpp>

class Food {
public:
  static unsigned int counter;

private:
  unsigned int m_id;

  Map *m_map;
  Position m_position;
  double m_radius;

public:
  Food(Map *map, Position position, double radius = 0.05)
      : m_map(map), m_position(position), m_radius(radius) {
    if (radius <= 0) {
      throw std::runtime_error("Invalid food radius");
    }
    counter++;
    m_id = counter;
  }

  inline bool can_eat(Position position_animal) const {
    return m_map->distance(position_animal, m_position) < m_radius;
  }

  unsigned int get_id() const { return m_id; }

  void draw(sf::RenderWindow *window, sf::Vector2f offset,
            double window_size) const {
    Position position_offset({offset.x, offset.y});
    Position position = position_offset + m_position * window_size;
    sf::CircleShape food(m_radius * window_size);
    food.setPosition({(float)position.x, (float)position.y});
    food.setFillColor(sf::Color(0, 255, 0, 127));
    food.setOrigin(m_radius * window_size, m_radius * window_size);
    window->draw(food);
  }
};

unsigned int Food::counter = 0;
