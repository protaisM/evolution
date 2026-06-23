#pragma once

// #include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>

#include "position.h"

class Map {
protected:
  bool m_has_safe_boundary;

public:
  virtual ~Map() {}

  //---------------------------------------------------------------//
  // virtual functions, every subclass should implement these
  virtual bool is_in(Position) const = 0;
  virtual Position rnd_position() const = 0;
  virtual Position project_on_map(Position) const = 0;
  virtual double distance(Position, Position) const = 0;
  virtual Position get_center() const = 0;
  virtual double get_radius() const = 0;
  virtual void draw(sf::RenderWindow *window, sf::Vector2f offset,
                    float map_size) const = 0;
  //---------------------------------------------------------------//

  bool has_safe_boundary() { return m_has_safe_boundary; }

protected:
  void draw_boundaries(sf::RenderWindow *window, sf::Vector2f offset,
                       float map_size) const {

    float space_right =
        sf::VideoMode::getDesktopMode().width - map_size - offset.x;
    sf::RectangleShape boundary_right(
        sf::Vector2f(space_right, sf::VideoMode::getDesktopMode().height));
    boundary_right.setPosition(map_size + offset.x, 0.0f);
    boundary_right.setFillColor(sf::Color::Black);

    float space_left = offset.x;
    sf::RectangleShape boundary_left(
        sf::Vector2f(space_left, sf::VideoMode::getDesktopMode().height));
    boundary_left.setPosition(0.0f, 0.0f);
    boundary_left.setFillColor(sf::Color::Black);

    float space_bottom =
        sf::VideoMode::getDesktopMode().height - map_size - offset.y;
    sf::RectangleShape boundary_bottom(sf::Vector2f(map_size, space_bottom));
    boundary_bottom.setPosition(offset.x, map_size + offset.y);
    boundary_bottom.setFillColor(sf::Color::Black);

    float space_top = offset.y;
    sf::RectangleShape boundary_top(sf::Vector2f(map_size, space_top));
    boundary_top.setPosition(offset.x, 0.0f);
    boundary_top.setFillColor(sf::Color::Black);

    window->draw(boundary_right);
    window->draw(boundary_left);
    window->draw(boundary_bottom);
    window->draw(boundary_top);
  }
};

class Square : public Map {
private:
  double m_side_length;

public:
  Square(double side_length, bool has_boundary = true)
      : m_side_length(side_length) {
    m_has_safe_boundary = has_boundary;
  }

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

  virtual double get_radius() const override { return m_side_length / 2; }

  virtual Position rnd_position() const override {
    Position result;
    result.x = ((double)rand() / ((double)RAND_MAX)) * m_side_length;
    result.y = ((double)rand() / ((double)RAND_MAX)) * m_side_length;
    return result;
  }

  virtual Position project_on_map(Position pos) const override {
    if (pos.x > m_side_length) {
      pos.x = m_side_length;
    }
    if (pos.y > m_side_length) {
      pos.y = m_side_length;
    }
    if (pos.x < 0) {
      pos.x = 0;
    }
    if (pos.y < 0) {
      pos.y = 0;
    }
    return pos;
  };

  virtual double distance(Position pos1, Position pos2) const override {
    return norm(pos1 - pos2);
  }

  virtual void draw(sf::RenderWindow *window, sf::Vector2f offset,
                    float map_size) const override {
    draw_boundaries(window, offset, map_size);
    if (m_has_safe_boundary) {
      sf::Color outline_color = sf::Color::White;
      sf::Vertex lines[] = {
          sf::Vertex(offset, outline_color),
          sf::Vertex(offset + sf::Vector2f({map_size, 0.0f}), outline_color),
          sf::Vertex(offset + sf::Vector2f({map_size, map_size}),
                     outline_color),
          sf::Vertex(offset + sf::Vector2f({0.0f, map_size}), outline_color),
          sf::Vertex(offset, outline_color)};
      window->draw(lines, 5, sf::LinesStrip);
    }
  }
};

class Circle : public Map {
private:
  double m_diameter;

public:
  Circle(double diameter, bool has_boundary = true) : m_diameter(diameter) {
    m_has_safe_boundary = has_boundary;
  }

  virtual bool is_in(Position pos) const override {
    if (distance(pos, get_center()) > (m_diameter / 2)) {
      return false;
    }
    return true;
  }

  virtual Position get_center() const override {
    Position center({m_diameter / 2, m_diameter / 2});
    return center;
  }

  virtual double get_radius() const override { return m_diameter / 2; }

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

  virtual void draw(sf::RenderWindow *window, sf::Vector2f offset,
                    float map_size) const override {
    draw_boundaries(window, offset, map_size);
    sf::CircleShape outside(map_size * m_diameter / 2);
    sf::Vector2f real_center =
        offset + sf::Vector2f({map_size * (float)get_center().x,
                               map_size * (float)get_center().y});
    outside.setPosition(real_center);
    outside.setOutlineColor(sf::Color::Black);
    outside.setFillColor(sf::Color::Transparent);
    outside.setOutlineThickness(2000.f);
    outside.setPointCount(200);
    outside.setOrigin(map_size * get_center().x,
                      map_size * (float)get_center().y);
    window->draw(outside);
    if (m_has_safe_boundary) {
      sf::CircleShape outline(map_size * m_diameter / 2);
      outline.setPosition(real_center);
      outline.setOutlineColor(sf::Color::White);
      outline.setFillColor(sf::Color::Transparent);
      outline.setOutlineThickness(2.f);
      outline.setPointCount(400);
      outline.setOrigin(map_size * get_center().x, map_size * get_center().y);
      window->draw(outline);
    }
  }
};

class Torus : public Map {
private:
  double m_side_length;

public:
  Torus(double side_length) : m_side_length(side_length) {
    m_has_safe_boundary = true;
  }

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

  virtual double get_radius() const override { return m_side_length / 2; }

  virtual Position rnd_position() const override {
    Position result;
    result.x = ((double)rand() / ((double)RAND_MAX)) * m_side_length;
    result.y = ((double)rand() / ((double)RAND_MAX)) * m_side_length;
    return result;
  }

  virtual Position project_on_map(Position pos) const override {
    Position offset({1, 1});
    return (pos + offset).mod(m_side_length);
  };

  virtual double distance(Position pos1, Position pos2) const override {
    return norm(pos1 - pos2);
  }

  virtual void draw(sf::RenderWindow *window, sf::Vector2f offset,
                    float map_size) const override {
    sf::Color outline_color = sf::Color::White;
    sf::Vertex lines[] = {
        sf::Vertex(offset, outline_color),
        sf::Vertex(offset + sf::Vector2f({map_size, 0.0f}), outline_color),
        sf::Vertex(offset + sf::Vector2f({map_size, map_size}), outline_color),
        sf::Vertex(offset + sf::Vector2f({0.0f, map_size}), outline_color),
        sf::Vertex(offset, outline_color)};
    window->draw(lines, 5, sf::Lines);
    draw_boundaries(window, offset, map_size);
  }
};
