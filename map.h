#pragma once

// #include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>

#include "position.h"

class Map {
protected:
  bool m_has_boundary;

public:
  virtual bool is_in(Position) const = 0;
  virtual Position rnd_position() const = 0;
  virtual Position project_on_map(Position) const = 0;
  virtual double distance(Position, Position) const = 0;
  virtual Position get_center() const = 0;
  virtual void draw(sf::RenderWindow *window, double window_size) const = 0;
  virtual double get_radius() const = 0;

  bool has_boundary() { return m_has_boundary; }
};

class Square : public Map {
private:
  double m_side_length;

public:
  Square(double side_length, bool has_boundary = true)
      : m_side_length(side_length) {
    m_has_boundary = has_boundary;
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

  virtual void draw(sf::RenderWindow *window,
                    double window_size) const override {
    sf::Vertex line_bottom[] = {
        sf::Vertex(sf::Vector2f(0.0f, window_size)),
        sf::Vertex(sf::Vector2f(window_size, window_size))};
    sf::Vertex line_right[] = {
        sf::Vertex(sf::Vector2f(window_size, 0.0f)),
        sf::Vertex(sf::Vector2f(window_size, window_size))};
    sf::Vertex line_top[] = {sf::Vertex(sf::Vector2f(1.0f, 1.0f)),
                             sf::Vertex(sf::Vector2f(1.0f, window_size))};
    sf::Vertex line_left[] = {sf::Vertex(sf::Vector2f(1.0f, 1.0f)),
                              sf::Vertex(sf::Vector2f(window_size, 1.0f))};
    window->draw(line_bottom, 2, sf::Lines);
    window->draw(line_right, 2, sf::Lines);
    window->draw(line_top, 2, sf::Lines);
    window->draw(line_left, 2, sf::Lines);
  };
};

class Circle : public Map {
private:
  double m_diameter;

public:
  Circle(double diameter, bool has_boundary = true) : m_diameter(diameter) {
    m_has_boundary = has_boundary;
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

  virtual void draw(sf::RenderWindow *window,
                    double window_size) const override {
    sf::CircleShape outside(window_size * m_diameter / 2);
    outside.setPosition(window_size * get_center().x,
                        window_size * get_center().y);
    outside.setOutlineColor(sf::Color::Black);
    outside.setFillColor(sf::Color::Transparent);
    outside.setOutlineThickness(1000.f);
    outside.setPointCount(200);
    outside.setOrigin(window_size * get_center().x,
                      window_size * get_center().y);
    window->draw(outside);
    // sf::CircleShape outline(window_size * m_diameter / 2);
    // outline.setPosition(window_size * get_center().x,
    //                     window_size * get_center().y);
    // outline.setOutlineColor(sf::Color::White);
    // outline.setFillColor(sf::Color::Transparent);
    // outline.setOutlineThickness(2.f);
    // outline.setPointCount(200);
    // outline.setOrigin(window_size * get_center().x,
    //                   window_size * get_center().y);
    // window->draw(outline);
  }
};

class Torus : public Map {
private:
  double m_side_length;

public:
  Torus(double side_length) : m_side_length(side_length) {
    m_has_boundary = true;
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

  virtual void draw(sf::RenderWindow *window,
                    double window_size) const override {
    sf::Vertex line_bottom[] = {
        sf::Vertex(sf::Vector2f(0.0f, window_size)),
        sf::Vertex(sf::Vector2f(window_size, window_size))};
    sf::Vertex line_right[] = {
        sf::Vertex(sf::Vector2f(window_size, 0.0f)),
        sf::Vertex(sf::Vector2f(window_size, window_size))};
    sf::Vertex line_top[] = {sf::Vertex(sf::Vector2f(1.0f, 1.0f)),
                             sf::Vertex(sf::Vector2f(1.0f, window_size))};
    sf::Vertex line_left[] = {sf::Vertex(sf::Vector2f(1.0f, 1.0f)),
                              sf::Vertex(sf::Vector2f(window_size, 1.0f))};
    window->draw(line_bottom, 2, sf::Lines);
    window->draw(line_right, 2, sf::Lines);
    window->draw(line_top, 2, sf::Lines);
    window->draw(line_left, 2, sf::Lines);
  };
};
