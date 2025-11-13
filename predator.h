#pragma once

#include "map.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include <functional>

struct Cat {
  Position m_position;
  double m_radius;
  double m_velocity = 0.1;
  double m_angle = 0.0;

  Cat(std::function<Position()> rnd_pos_generator, double radius)
      : m_radius(radius), m_position(rnd_pos_generator()) {}

  Cat() {}

  Position get_position() { return m_position; }

  void advance(double dt, std::function<bool(Position)> is_in_map) {
    m_angle = M_PI / 2 + atan2((m_position.y - 0.5) , (m_position.x - 0.5));
    Position pos({m_position.x, m_position.y});
    pos.x += dt * std::cos(m_angle) * m_velocity;
    pos.y += dt * std::sin(m_angle) * m_velocity;
    if (is_in_map(pos)) {
      m_position = pos;
    }
  }
  bool is_in_death_zone(Position pos) {
    if ((m_position.x - pos.x) * (m_position.x - pos.x) +
            (m_position.y - pos.y) * (m_position.y - pos.y) <
        m_radius * m_radius) {
      return true;
    }
    return false;
  }

  void draw(sf::RenderWindow *window, double window_size) const {
    sf::CircleShape death(m_radius * window_size);
    death.setPosition(m_position.x * window_size, m_position.y * window_size);
    death.setFillColor(sf::Color::Red);
    death.setOrigin(m_radius * window_size, m_radius * window_size);
    window->draw(death);
  }
};
