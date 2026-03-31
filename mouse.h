#pragma once

#include "brain.h"
#include "color.h"
#include "eggshape.hpp"
#include "map.h"
#include "position.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

inline double rand_pos() { return (double)std::rand() / ((double)RAND_MAX); }
inline double rand_angle() { return 2 * M_PI * rand_pos(); }

template <unsigned int NB_IN_NODES, unsigned int NB_OUT_NODES,
          unsigned int MAX_BRAIN_SIZE, unsigned int MAX_NB_CONNECTIONS>
class BaseMouse {

protected:
  //---------------------------------------------------------------//
  // virtual functions, every subclass should implement these
  virtual void
  update_angle_and_velocity(std::vector<PositionAngle> predators_states,
                            double dt) = 0;

  virtual std::string specific_informations() const = 0;
  //---------------------------------------------------------------//

protected:
  Brain<NB_IN_NODES, NB_OUT_NODES, MAX_BRAIN_SIZE, MAX_NB_CONNECTIONS> m_brain;
  Map *m_map;
  // note that positions and velocity are normalised to 1
  PositionAngle m_state;
  double m_velocity;

  // normalised to 1
  double m_sight_radius;
  bool m_is_alive;
  Color m_color;

public:
  BaseMouse(Map *map)
      : m_brain(), m_map(map), m_state({map->rnd_position(), rand_angle()}),
        m_velocity(0), m_is_alive(true) {
    m_sight_radius = rand_0_1();
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

  void print() {
    std::cout << specific_informations() << std::endl;
    std::cout << informations() << std::endl;
  }

  std::string informations() const {
    std::string result;
    result += "The mouse is at position (" +
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

  bool advance(double dt, std::vector<PositionAngle> predators_states) {
    update_angle_and_velocity(predators_states, dt);
    return update_position(dt);
  }

  void mutate() {
    m_brain.mutate();
    m_sight_radius += rand_normal();
    if (m_sight_radius < 0) {
      m_sight_radius = 0;
    }
    m_color.mutate();
  }

protected:
  Position get_next_position(double dt) {
    Position direction({std::cos(m_state.angle), std::sin(m_state.angle)});
    return m_state.position + dt * m_velocity * direction;
  }

  bool update_position(double dt) {
    // given the new angle and the new velocity, update the position
    // return false if the new position is outside the map and the mouse died
    if (!m_is_alive) {
      std::cerr << "This mouse is not alive" << std::endl;
    }
    Position next_pos(get_next_position(dt));
    if (m_map->is_in(next_pos)) {
      m_state.position = next_pos;
    } else if (m_map->has_safe_boundary()) {
      m_state.position = m_map->project_on_map(next_pos);
    } else {
      kill();
      return false;
    }
    return true;
  }

public:
  void draw(sf::RenderWindow *window, sf::Vector2f offset, double zoom,
            bool selected) const {
    sf::Color outline = sf::Color::Black;
    float transparency = 255;
    if (selected) {
      transparency = 255;
      outline = sf::Color::Red;
    }

    sf::Vector2f position({(float)zoom * (float)m_state.position.x,
                           (float)zoom * (float)m_state.position.y});
    position = position + offset;
    Eggshape body;
    float radius = 7;
    body.setSize(radius);
    body.setElongation(5 + radius * m_velocity / 2);
    body.setPosition(position);
    body.setOrigin(radius, radius);
    body.setFillColor(sf::Color(m_color.r, m_color.g, m_color.b, transparency));
    body.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    body.setOutlineThickness(1.0f);
    if (selected) {
      body.setOutlineThickness(5.0f);
    }
    body.setOutlineColor(outline);

    sf::CircleShape left_eye;
    left_eye.setRadius(2);
    left_eye.setPosition(position);
    left_eye.setOrigin(2 + radius / 2, 2 + radius / 2 + m_velocity);
    left_eye.setFillColor(sf::Color::White);
    left_eye.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    left_eye.setOutlineThickness(1.0f);
    left_eye.setOutlineColor(sf::Color::Black);

    sf::CircleShape right_eye;
    right_eye.setRadius(2);
    right_eye.setPosition(position);
    right_eye.setOrigin(2 - radius / 2, 2 + radius / 2 + m_velocity);
    right_eye.setFillColor(sf::Color::White);
    right_eye.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    right_eye.setOutlineThickness(1.0f);
    right_eye.setOutlineColor(sf::Color::Black);

    sf::CircleShape left_pupil;
    left_pupil.setRadius(1);
    left_pupil.setPosition(position);
    left_pupil.setOrigin(1 + radius / 2, 2 + radius / 2 + m_velocity);
    left_pupil.setFillColor(sf::Color::Black);
    left_pupil.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);

    sf::CircleShape right_pupil;
    right_pupil.setRadius(1);
    right_pupil.setPosition(position);
    right_pupil.setOrigin(1 - radius / 2, 2 + radius / 2 + m_velocity);
    right_pupil.setFillColor(sf::Color::Black);
    right_pupil.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);

    sf::CircleShape left_ear;
    left_ear.setRadius(5);
    left_ear.setPosition(position);
    left_ear.setOrigin(7 + radius / 2, radius / 2);
    left_ear.setFillColor(
        sf::Color(std::min(m_color.r + 30, 255), std::min(m_color.g + 30, 255),
                  std::min(m_color.b + 30, 255), transparency));
    left_ear.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    left_ear.setOutlineThickness(1.0f);
    left_ear.setOutlineColor(sf::Color(50, 50, 50));

    sf::CircleShape right_ear;
    right_ear.setRadius(5);
    right_ear.setPosition(position);
    right_ear.setOrigin(5 - 2 - radius / 2, radius / 2);
    right_ear.setFillColor(
        sf::Color(std::min(m_color.r + 30, 255), std::min(m_color.g + 30, 255),
                  std::min(m_color.b + 30, 255), transparency));
    right_ear.rotate((M_PI / 2 + m_state.angle) / (2 * M_PI) * 360);
    right_ear.setOutlineThickness(1.0f);
    right_ear.setOutlineColor(sf::Color(50, 50, 50));

    window->draw(body);
    window->draw(left_ear);
    window->draw(right_ear);
    window->draw(left_eye);
    window->draw(right_eye);
    window->draw(left_pupil);
    window->draw(right_pupil);
  }
};

template <unsigned int NB_PREDATORS>
class SimpleMouse : public BaseMouse<3 + 3 * NB_PREDATORS, 2, 30, 100> {
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
  SimpleMouse(Map *map) : BaseMouse<m_nb_input, 2, 30, 100>(map) {}

  SimpleMouse() : BaseMouse<m_nb_input, 2, 30, 100>() {}

protected:
  virtual std::string specific_informations() const override {
    std::string result;
    result += "I am a simple mouse!";
    return result;
  }

  virtual void
  update_angle_and_velocity(std::vector<PositionAngle> predators_states,
                            double dt) override {
    std::sort(
        predators_states.begin(), predators_states.end(),
        [this](const PositionAngle &lhs, const PositionAngle &rhs) {
          return this->m_map->distance(this->m_state.position, lhs.position) <
                 this->m_map->distance(this->m_state.position, rhs.position);
        });

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
class MemoryMouse : public BaseMouse<3 + 2 * NB_PREDATORS, 3, 30, 150> {
  /* A memory mouse is a bit more complex:
   * we add a new input, a memory neuron, which
   * can store information. It therefore adds a input
   * node, and an output one.
   * We increase the number of possible neurons to 30,
   * and the number of connections to 150.
   */
};

class TimeRobot : public BaseMouse<1, 2, 30, 100> {

private:
  double m_internal_clock;

public:
  TimeRobot(Map *map) : BaseMouse<1, 2, 30, 100>(map), m_internal_clock(0.0) {}

  TimeRobot() : BaseMouse<1, 2, 30, 100>(), m_internal_clock(0.0) {}

  virtual std::string specific_informations() const override {
    std::string result;
    result += "I am a time robot!\n";
    result += "My internal clock is " + std::to_string(m_internal_clock);
    return result;
  }

protected:
  virtual void update_angle_and_velocity(std::vector<PositionAngle>,
                                         double dt) override {
    // FIXME: we actually don't need the predators
    m_internal_clock += dt;
    std::array<double, 2> output_from_brain;
    output_from_brain = (this->m_brain).activate({m_internal_clock});

    this->m_velocity = std::abs(output_from_brain[0]);
    this->m_state.angle += dt * M_PI * output_from_brain[1];
    this->m_state.angle = std::fmod(this->m_state.angle, 2 * M_PI);
  }
};
