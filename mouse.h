#pragma once

#include "brain.h"
#include "ellipse.hpp"
#include "map.h"
#include "position.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <array>
#include <cmath>
#include <cstdlib>
#include <string>

struct Color {
  int r;
  int g;
  int b;
};

inline double rand_pos() { return (double)std::rand() / ((double)RAND_MAX); }
inline double rand_angle() { return 2 * M_PI * rand_pos(); }

template <unsigned int NB_IN_NODES, unsigned int NB_OUT_NODES,
          unsigned int MAX_BRAIN_SIZE, unsigned int MAX_NB_CONNECTIONS,
          unsigned int NB_PREDATORS>
class BaseMouse {
protected:
  Brain<NB_IN_NODES, NB_OUT_NODES, MAX_BRAIN_SIZE, MAX_NB_CONNECTIONS> m_brain;
  Map *m_map;
  // note that positions and velocity are normalised to 1
  PositionAngle m_state;
  double m_velocity;
  // the angle is in [0,2pi]
  // double m_angle;

  // normalised to 1
  double m_sight_radius;
  bool m_is_alive;
  Color m_color;

public:
  BaseMouse(Map *map, double sight_radius)
      : m_brain(), m_map(map), m_state({map->rnd_position(), rand_angle()}),
        m_velocity(0), m_sight_radius(sight_radius), m_is_alive(true) {
    m_color.r = std::rand() % 255;
    m_color.g = std::rand() % 255;
    m_color.b = std::rand() % 255;
  }

  BaseMouse() : m_is_alive(false) {}

  Position get_position() const { return m_state.position; }
  double get_angle() const { return m_state.angle; }
  bool is_alive() const { return m_is_alive; }
  void kill() { m_is_alive = false; }
  void resurrect() { m_is_alive = true; }
  void randomize_position() { m_state.position = m_map->rnd_position(); }

  double get_sight_radius() const { return m_sight_radius; }
  double get_velocity() const { return m_velocity; }

protected:
  Position get_next_position(double dt) {
    Position direction({std::cos(m_state.angle), std::sin(m_state.angle)});
    return m_state.position + dt * m_velocity * direction;
  }

  bool update_position(double dt) {
    // given the new angle and the new velocity, update the position
    // return false if the new position is outside the map and the mouse died
    if (!m_is_alive) {
      std::cerr << "This bird is not alive" << std::endl;
    }
    Position next_pos(get_next_position(dt));
    if (m_map->is_in(next_pos)) {
      m_state.position = next_pos;
    } else if (m_map->has_boundary()) {
      m_state.position = m_map->project_on_map(next_pos);
    } else {
      kill();
      return false;
    }
    return true;
  }

  virtual void update_angle_and_velocity(
      std::array<PositionAngle, NB_PREDATORS> predators_states, double dt) = 0;

  virtual void print() { std::cout << informations() << std::endl; }

  virtual std::string informations() const {
    return "I'm just a base mouse, surely this is a mistake!\n";
  }

public:
  bool advance(double dt,
               std::array<PositionAngle, NB_PREDATORS> predators_states) {
    update_angle_and_velocity(predators_states, dt);
    return update_position(dt);
  }

  void mutate() {
    m_brain.mutate();
    m_sight_radius += rand_normal();
    if (m_sight_radius < 0) {
      m_sight_radius = 0;
    }
    unsigned int rnd = rnd_int_smaller_than(6);
    switch (rnd) {
    case 0: {
      m_color.r += 1;
      break;
    }
    case 1: {
      m_color.r -= 1;
      break;
    }
    case 2: {
      m_color.g += 1;
      break;
    }
    case 3: {
      m_color.g -= 1;
      break;
    }
    case 4: {
      m_color.b += 1;
      break;
    }
    case 5: {
      m_color.b -= 1;
      break;
    }
    }
  }

  void draw(sf::RenderWindow *window, double zoom) const {
    Eggshape to_display;
    float radius = 7;
    to_display.setSize(radius);
    to_display.setElongation(5 + radius * m_velocity / 2);
    to_display.setPosition(zoom * m_state.position.x,
                           zoom * m_state.position.y);
    to_display.setOrigin(radius, radius);
    to_display.setFillColor(sf::Color(m_color.r, m_color.g, m_color.b));
    to_display.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    to_display.setOutlineThickness(1.0f);
    to_display.setOutlineColor(sf::Color::Black);

    sf::CircleShape left_eye;
    left_eye.setRadius(2);
    left_eye.setPosition(zoom * m_state.position.x, zoom * m_state.position.y);
    left_eye.setOrigin(2 + radius / 2, 2 + radius / 2 + m_velocity);
    left_eye.setFillColor(sf::Color::White);
    left_eye.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    left_eye.setOutlineThickness(1.0f);
    left_eye.setOutlineColor(sf::Color::Black);

    sf::CircleShape right_eye;
    right_eye.setRadius(2);
    right_eye.setPosition(zoom * m_state.position.x, zoom * m_state.position.y);
    right_eye.setOrigin(2 - radius / 2, 2 + radius / 2 + m_velocity);
    right_eye.setFillColor(sf::Color::White);
    right_eye.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    right_eye.setOutlineThickness(1.0f);
    right_eye.setOutlineColor(sf::Color::Black);

    sf::CircleShape left_pupil;
    left_pupil.setRadius(1);
    left_pupil.setPosition(zoom * m_state.position.x,
                           zoom * m_state.position.y);
    left_pupil.setOrigin(1 + radius / 2, 2 + radius / 2 + m_velocity);
    left_pupil.setFillColor(sf::Color::Black);
    left_pupil.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);

    sf::CircleShape right_pupil;
    right_pupil.setRadius(1);
    right_pupil.setPosition(zoom * m_state.position.x,
                            zoom * m_state.position.y);
    right_pupil.setOrigin(1 - radius / 2, 2 + radius / 2 + m_velocity);
    right_pupil.setFillColor(sf::Color::Black);
    right_pupil.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);

    sf::CircleShape left_ear;
    left_ear.setRadius(5);
    left_ear.setPosition(zoom * m_state.position.x, zoom * m_state.position.y);
    left_ear.setOrigin(7 + radius / 2, radius / 2);
    left_ear.setFillColor(sf::Color(std::min(m_color.r + 30, 255),
                                    std::min(m_color.g + 30, 255),
                                    std::min(m_color.b + 30, 255)));
    left_ear.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    left_ear.setOutlineThickness(1.0f);
    left_ear.setOutlineColor(sf::Color::Black);

    sf::CircleShape right_ear;
    right_ear.setRadius(5);
    right_ear.setPosition(zoom * m_state.position.x, zoom * m_state.position.y);
    right_ear.setOrigin(5 - 2 - radius / 2, radius / 2);
    right_ear.setFillColor(sf::Color(std::min(m_color.r + 30, 255),
                                     std::min(m_color.g + 30, 255),
                                     std::min(m_color.b + 30, 255)));
    right_ear.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    right_ear.setOutlineThickness(1.0f);
    right_ear.setOutlineColor(sf::Color::Black);

    window->draw(to_display);
    window->draw(left_ear);
    window->draw(right_ear);
    window->draw(left_eye);
    window->draw(right_eye);
    window->draw(left_pupil);
    window->draw(right_pupil);
  }
};

template <unsigned int NB_PREDATORS>
class SimpleMouse
    : public BaseMouse<3 + 3 * NB_PREDATORS, 2, 30, 100, NB_PREDATORS> {
  /* A simple mouse has a small brain:
   * it can simply handle the distance to the predator,
   * and the angle at which it sees it. Of course it is also conscious
   * of its own position. It outputs the new velocity,
   * and the change in angle.
   * We cap the number of neurons to 20, and the number of
   * connections to 100.
   */

private:
  unsigned int static constexpr m_nb_input = 3 + 3 * NB_PREDATORS;

public:
  SimpleMouse(Map *map, double sight_radius = 0.5)
      : BaseMouse<m_nb_input, 2, 30, 100, NB_PREDATORS>(map, sight_radius) {}

  SimpleMouse() : BaseMouse<m_nb_input, 2, 30, 100, NB_PREDATORS>() {}

  virtual void print() override { std::cout << informations(); }

  virtual std::string informations() const override {
    std::string result;
    result += "A simple mouse at position (" +
              std::to_string(this->m_state.position.x) + "," +
              std::to_string(this->m_state.position.y) + "), with velocity " +
              std::to_string(this->m_velocity) + " and direction " +
              std::to_string(this->m_state.angle) + "\n";
    if (this->m_is_alive) {
      result += "It is alive!!\n";
    }
    result += "Its brain is:\n";
    result += this->m_brain.informations();
    return result;
  }

protected:
  virtual void update_angle_and_velocity(
      std::array<PositionAngle, NB_PREDATORS> predators_states,
      double dt) override {

    std::array<double, m_nb_input> input_to_brain;
    std::array<double, 2> output_from_brain;
    for (unsigned int i = 0; i < NB_PREDATORS; i++) {
      PositionAngle predator_state = predators_states[i];
      double dist = this->m_map->distance(this->m_state.position,
                                          predator_state.position);
      double angle_with_pred =
          angle(this->m_state.position, predator_state.position);
      double angle_of_pred = predator_state.angle - angle_with_pred;
      if (dist > this->m_sight_radius) {
        dist = -1;
        angle_with_pred = 0;
        angle_of_pred = 0;
      }
      input_to_brain[3 * i] = dist;
      input_to_brain[3 * i + 1] = angle_with_pred;
      input_to_brain[3 * i + 2] = angle_of_pred;
    }
    input_to_brain[3 * NB_PREDATORS] = this->m_state.position.x;
    input_to_brain[3 * NB_PREDATORS + 1] = this->m_state.position.y;
    input_to_brain[3 * NB_PREDATORS + 2] = this->m_state.angle;
    output_from_brain = (this->m_brain).activate(input_to_brain);

    this->m_velocity = std::abs(output_from_brain[0]);
    this->m_state.angle += dt * M_PI * output_from_brain[1];
    this->m_state.angle = std::fmod(this->m_state.angle, 2 * M_PI);
  }
};

template <unsigned int NB_PREDATORS>
class MemoryMouse
    : public BaseMouse<3 + 2 * NB_PREDATORS, 3, 30, 150, NB_PREDATORS> {
  /* A memory mouse is a bit more complex:
   * we add a new input, a memory neuron, which
   * can store information. It therefore adds a input
   * node, and an output one.
   * We increase the number of possible neurons to 30,
   * and the number of connections to 150.
   */
};
