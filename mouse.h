#pragma once

#include "brain.h"
#include "map.h"

// #include <SFML/Graphics.hpp>
// #include <SFML/Graphics/RenderWindow.hpp>
#include <array>
#include <cmath>
#include <functional>

struct Color {
  int r;
  int g;
  int b;
};

inline double rand_pos() { return (double)std::rand() / ((double)RAND_MAX); }
inline double rand_angle() { return 2 * M_PI * rand_pos(); }

template <unsigned int NB_IN_NODES, unsigned int NB_OUT_NODES,
          unsigned int MAX_BRAIN_SIZE, unsigned int MAX_NB_CONNECTIONS>
class BaseMouse {
protected:
  Brain<NB_IN_NODES, NB_OUT_NODES, MAX_BRAIN_SIZE, MAX_NB_CONNECTIONS> m_brain;
  // note that positions and velocity are normalised to 1
  Position m_position;
  double m_velocity;
  // the angle is in [0,2pi]
  double m_angle;

  // normalised to 1
  double m_sight_radius;
  // in [0,360]
  double m_max_angle;
  bool m_is_alive;
  Color m_color;

public:
  BaseMouse(double max_angle, double sight_radius,
            std::function<Position()> rnd_pos_generator)
      : m_position(rnd_pos_generator()), m_is_alive(true), m_velocity(0),
        m_max_angle(max_angle), m_angle(rand_angle()),
        m_sight_radius(sight_radius), m_brain(3) {
    m_color.r = std::rand() % 255;
    m_color.g = std::rand() % 255;
    m_color.b = std::rand() % 255;
  }

  BaseMouse() : m_is_alive(false) {}

  double get_x() const { return m_position.x; }
  double get_y() const { return m_position.y; }
  double get_angle() const { return m_angle; }
  bool is_alive() const { return m_is_alive; }
  void kill() { m_is_alive = false; }

protected:
  Position get_next_position(double dt) {
    Position pos({m_position.x, m_position.y});
    pos.x += dt * std::cos(m_angle) * m_velocity;
    pos.y += dt * std::sin(m_angle) * m_velocity;
    return pos;
  }

  void update_position(double dt, std::function<bool(Position)> is_in_map) {
    // given the new angle and the new velocity, update the position
    if (!m_is_alive) {
      std::cerr << "This bird is not alive" << std::endl;
    }
    Position next_pos(get_next_position(dt));
    if (is_in_map(next_pos)) {
      m_position.x = next_pos.x;
      m_position.y = next_pos.y;
    }
  }

  // void draw(sf::RenderWindow *window) const {
  //   sf::CircleShape to_display;
  //   to_display.setRadius(5);
  //   to_display.setPosition(m_x_pos, m_y_pos);
  //   to_display.setOrigin(5.0f, 5.0f);
  //   to_display.setFillColor(sf::Color(m_color.r, m_color.g, m_color.b));
  //   window->draw(to_display);
  // }

  virtual void update_angle_and_velocity(std::array<double, NB_IN_NODES>) = 0;

  virtual void print() {
    std::cout << "I'm just a base mouse, surely this is a mistake!"
              << std::endl;
  }

public:
  void advance(double dt, std::array<double, NB_IN_NODES> input,
               std::function<bool(Position)> is_in_map) {
    update_angle_and_velocity(input);
    update_position(dt, is_in_map);
  }

  void mutate(double mutation_strength) { m_brain.mutate(mutation_strength); }
};

class SimpleMouse : public BaseMouse<2, 2, 20, 100> {
  /* A simple mouse has a small brain:
   * it can simply handle the distance to the predator,
   * and the angle at which it sees it. It outputs the new velocity,
   * and the change in angle.
   * We cap the number of neurons to 20, and the number of
   * connections to 100.
   */

public:
  SimpleMouse(double max_angle, double sight_radius,
              std::function<Position()> rnd_pos_generator)
      : BaseMouse<2, 2, 20, 100>(max_angle, sight_radius, rnd_pos_generator) {}

  SimpleMouse() : BaseMouse<2, 2, 20, 100>() {}

  virtual void print() override {
    std::cout << "A simple mouse at position (" << m_position.x << ","
              << m_position.y << "), with velocity " << m_velocity
              << " and direction " << m_angle << std::endl;
    std::cout << "Its brain is:" << std::endl;
    m_brain.print();
  }

protected:
  virtual void
  update_angle_and_velocity(std::array<double, 2> predator_position) override {
    double dist_x = predator_position[0] - m_position.x;
    double dist_y = predator_position[1] - m_position.y;
    double dist = std::sqrt((dist_x) * (dist_x) + (dist_y) * (dist_y));
    std::array<double, 2> output_from_brain;
    if (dist <= m_sight_radius) {
      output_from_brain = m_brain.activate({dist, std::atan(dist_y / dist_x)});
    } else {
      output_from_brain = m_brain.activate({-1, 0});
    }
    m_velocity = std::abs(output_from_brain[0]);
    m_angle += m_max_angle * output_from_brain[1];
    if (m_angle < 0)
      m_angle += 2 * M_PI;
    if (m_angle > 2 * M_PI)
      m_angle -= 2 * M_PI;
  }
};

class MemoryMouse : public BaseMouse<3, 3, 30, 150> {
  /* A memory mouse is a bit more complex:
   * we add a new input, a memory neuron, which
   * can store information. It therefore adds a input
   * node, and an output one.
   * We increase the number of possible neurons to 30,
   * and the number of connections to 150.
   */
};
