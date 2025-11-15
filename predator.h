#pragma once

#include "map.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>

namespace Predator {
inline double rand_angle() {
  return 2 * M_PI * ((double)rand() / (double)RAND_MAX);
}

struct Run_in_circle {
  Position m_position;
  Map *m_map;
  double m_radius;
  double m_velocity = 0.2;
  double m_angle = 0.0;

  Run_in_circle(Map *map, double radius)
      : m_position(map->rnd_position()), m_map(map), m_radius(radius) {}

  Run_in_circle() {}

  Position get_position() { return m_position; }

  void advance(double dt) {
    m_angle = M_PI / 2 + atan2((m_position.y - 0.5), (m_position.x - 0.5));
    Position direction({std::cos(m_angle), std::sin(m_angle)});
    Position pos = m_position + dt * m_velocity * direction;
    if (m_map->is_in(pos)) {
      m_position = pos;
    } else {
      m_position = m_map->project_on_map(pos);
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

  void randomize_position() { m_position = m_map->rnd_position(); }

  void draw(sf::RenderWindow *window, double window_size) const {
    sf::CircleShape death(m_radius * window_size);
    death.setPosition(m_position.x * window_size, m_position.y * window_size);
    death.setFillColor(sf::Color::Red);
    death.setOrigin(m_radius * window_size, m_radius * window_size);
    window->draw(death);
  }
};

struct Bounce {
  Position m_position;
  Map *m_map;
  double m_radius;
  double m_velocity = 0.2;
  double m_angle;

  Bounce(Map *map, double radius)
      : m_position(map->rnd_position()), m_map(map), m_radius(radius),
        m_angle(rand_angle()) {}

  Bounce() {}

  Position get_position() { return m_position; }

  void advance(double dt) {
    Position direction({std::cos(m_angle), std::sin(m_angle)});
    Position pos = m_position + dt * m_velocity * direction;
    if (m_map->is_in(pos)) {
      m_position = pos;
    } else {
      m_position = m_map->project_on_map(pos);
      m_angle = rand_angle();
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

  void randomize_position() {}

  void draw(sf::RenderWindow *window, double window_size) const {
    sf::CircleShape death(m_radius * window_size);
    death.setPosition(m_position.x * window_size, m_position.y * window_size);
    death.setFillColor(sf::Color::Red);
    death.setOrigin(m_radius * window_size, m_radius * window_size);
    window->draw(death);
  }
};

struct Straigth {
  Position m_position;
  Map *m_map;
  double m_radius;
  double m_velocity = 0.2;
  double m_angle;

  Straigth(Map *map, double radius)
      : m_position(map->rnd_position()), m_map(map), m_radius(radius),
        m_angle(rand_angle()) {}

  Straigth() {}

  Position get_position() { return m_position; }

  void advance(double dt) {
    Position direction({std::cos(m_angle), std::sin(m_angle)});
    Position pos = m_position + dt * m_velocity * direction;
    if (m_map->is_in(pos)) {
      m_position = pos;
    } else {
      m_position = m_map->project_on_map(pos);
    }
    m_angle += dt * (rand_angle() - M_PI);
  }
  bool is_in_death_zone(Position pos) {
    if ((m_position.x - pos.x) * (m_position.x - pos.x) +
            (m_position.y - pos.y) * (m_position.y - pos.y) <
        m_radius * m_radius) {
      return true;
    }
    return false;
  }

  void randomize_position() {}

  void draw(sf::RenderWindow *window, double window_size) const {
    sf::CircleShape death(m_radius * window_size);
    death.setPosition(m_position.x * window_size, m_position.y * window_size);
    death.setFillColor(sf::Color::Red);
    death.setOrigin(m_radius * window_size, m_radius * window_size);
    window->draw(death);
  }
};
}; // namespace Predator
