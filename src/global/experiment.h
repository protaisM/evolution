#pragma once

#include "display_parameter.h"
#include "experiment_display.h"
#include "experiment_parameter.h"
#include "experiment_rules.h"
#include "food.h"
#include "level.h"
#include "logger.h"
#include "map.h"
#include "position.h"
#include "predator.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <array>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

class ExperimentController;

template <typename Player> class IExperiment {};

template <typename Mouse> class Experiment : public IExperiment<Mouse> {
private:
  Logger *const m_logger;

  std::unique_ptr<ExperimentRules<Mouse>> m_experiment_rules;
  Level m_level;

  std::vector<Mouse> m_mice;

  // parameters
  ExperimentParameters m_params;

  friend class ExperimentController;
  friend class ExperimentDisplay<Mouse>;

public:
  Experiment(Logger *log, Map *map, ExperimentParameters const &params)
      : m_logger(log), m_level(map, 2), m_params(params) {

    // reproduction strategy
    m_experiment_rules = std::make_unique<FitnessFunction<Mouse>>();

    // mice
    for (unsigned int i = 0; i < m_params.maximal_mice_number; i++) {
      m_mice.emplace_back(map);
      if (!m_params.randomized_spawning_point) {
        m_mice[i].set_position(m_params.spawning_point);
        m_mice[i].set_angle(m_params.spawning_angle);
      }
    }
  }

  ~Experiment() = default;

  void do_one_step() {
    m_params.time = m_params.time + m_params.dt;
    std::vector<PositionAngle> predators_states;
    for (std::unique_ptr<Predator> const &predator : m_level.m_predators) {
      predator->do_one_step(m_params.dt);
      predators_states.push_back(predator->get_state());
    }
    for (Mouse &mouse : m_mice) {
      if (!mouse.is_alive()) {
        continue;
      }
      bool outside_the_map = !mouse.advance(m_params.dt, predators_states);
      if (outside_the_map) {
        m_experiment_rules->if_outside_map(mouse, m_params);
      }
      for (std::unique_ptr<Predator> const &predator : m_level.m_predators) {
        if (predator->is_in_predator(mouse.get_position())) {
          m_experiment_rules->if_in_predator(mouse, m_params);
        }
      }
      for (Food const &food : m_level.m_food) {
        if (food.can_eat(mouse.get_position())) {
          m_experiment_rules->if_eat_food(mouse, food);
        }
      }
    }

    if (!m_experiment_rules->condition_end_generation(m_params)) {
      return;
    }
    double avg_fitness = 0;
    double max_fitness = 0;
    for (Mouse const &mouse : m_mice) {
      avg_fitness += mouse.get_fitness() / m_params.maximal_mice_number;
      if (mouse.get_fitness() > max_fitness) {
        max_fitness = mouse.get_fitness();
      }
    }
    m_logger->store(
        m_params.generation, m_params.time / m_params.generation_duration,
        (double)m_params.nb_alive_mice / (double)m_params.maximal_mice_number,
        max_fitness);
    m_experiment_rules->reproduce(m_mice, m_params);
    for (Mouse &mouse : m_mice) {
      mouse.start_of_round();
    }
    for (std::unique_ptr<Predator> const &predator : m_level.m_predators) {
      predator->start_of_the_round();
    }
  }

  void add_to_dt(double dt) { m_params.dt = std::max(0.005, m_params.dt + dt); }

  void add_to_generation_duration(double dg) {
    m_params.generation_duration =
        std::max(1., m_params.generation_duration + dg);
  }

  void add_to_minimal_mice_number(double dn) {
    m_params.minimal_mice_number =
        std::max(10., m_params.minimal_mice_number + dn);
  }
};
