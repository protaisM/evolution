#pragma once

#include "map.h"
#include "position.h"
#include "predator_shape.h"
#include "predator_strategy.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdlib>
#include <memory>

class Predator {

private:
  PositionAngle m_state;
  double m_internal_clock;

  std::unique_ptr<PredatorShape> m_shape;
  std::unique_ptr<PredatorStrategy> m_strategy;

public:
  Predator(std::unique_ptr<PredatorShape> shape,
           std::unique_ptr<PredatorStrategy> strategy)
      : m_shape(std::move(shape)), m_strategy(std::move(strategy)) {}

  ~Predator() = default;

  Predator(const Predator &) = delete;
  Predator &operator=(const Predator &) = delete;

  Predator(Predator &&) = default;
  Predator &operator=(Predator &&) = default;

  bool is_in_predator(Position pos) const {
    if (m_shape->is_in(m_state.position, pos)) {
      return m_strategy->killing_strategy(m_internal_clock);
    }
    return false;
  }

  void draw(sf::RenderWindow *window, sf::Vector2f offset,
            double window_size) const {
    Position position_offset({offset.x, offset.y});
    m_shape->draw(window, position_offset + m_state.position * window_size,
                  window_size);
  }

  void start_of_the_round() {
    m_internal_clock = 0.0;
    m_state = m_strategy->start_of_the_round(m_state);
  }

  void do_one_step(double dt) {
    m_internal_clock += dt;
    m_state = m_strategy->advance(m_state, m_internal_clock, dt);
  }

  Position get_position() const { return m_state.position; }
  PositionAngle get_state() const { return m_state; }
};
