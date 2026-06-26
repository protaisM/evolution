#pragma once

#include "map.h"
#include "position.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

class PredatorShape {
  // ------------------------------------------------------------------------ //
  // virtual functions, every subclass should implement these
public:
  virtual bool is_in(Position pos_predator, Position pos) const = 0;
  virtual void draw(sf::RenderWindow *window, Position offset,
                    double window_size) const = 0;
  // ------------------------------------------------------------------------ //

  virtual ~PredatorShape() {};

protected:
  Map *m_map;

public:
  PredatorShape(Map *map) : m_map(map) {}
};

class Circle : public PredatorShape {
private:
  double m_radius;

public:
  Circle(Map *map, double radius = 0.1) : PredatorShape(map), m_radius(radius) {}

  bool is_in(Position pos_predator, Position pos) const override {
    if (m_map->distance(pos, pos_predator) < m_radius) {
      return true;
    }
    return false;
  }

  void draw(sf::RenderWindow *window, Position offset,
            double window_size) const override {
    sf::Vector2f position({(float)offset.x, (float)offset.y});
    sf::CircleShape death(m_radius * window_size);
    death.setPosition(position);
    death.setFillColor(sf::Color(255, 0, 0, 127));
    death.setOrigin(m_radius * window_size, m_radius * window_size);
    window->draw(death);
  }
};

class Rectangle : public PredatorShape {
private:
  double m_x_length;
  double m_y_length;

public:
  Rectangle(Map *map, double x_length, double y_length)
      : PredatorShape(map), m_x_length(x_length), m_y_length(y_length) {}

  bool is_in(Position pos_predator, Position pos) const override {
    if (std::abs(pos_predator.x - pos.x) < m_x_length / 2 and
        std::abs(pos_predator.y - pos.y) < m_y_length / 2) {
      return true;
    }
    return false;
  }

  void draw(sf::RenderWindow *window, Position offset,
            double window_size) const override {
    sf::Vector2f position({(float)offset.x, (float)offset.y});
    sf::RectangleShape death;
    death.setSize({static_cast<float>(m_x_length * window_size),
                   static_cast<float>(m_y_length * window_size)});
    death.setPosition(position);
    death.setFillColor(sf::Color{255, 0, 0, 127});
    death.setOrigin(m_x_length * window_size / 2, m_y_length * window_size / 2);
    window->draw(death);
  }
};
