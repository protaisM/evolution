#pragma once

#include "experiment_display.h"
#include "experiment_helper.h"
#include "experiment_parameter.h"
#include "food.h"
#include "level.h"
#include "logger.h"
#include "map.h"
#include "position.h"

#include <cstring>

class ExperimentController;

template <typename Mouse> class Experiment {

  friend class ExperimentController;
  friend class ExperimentDisplay<Mouse>;

private:
  std::vector<Mouse> m_mice;
  Level m_level;
  ExperimentParameters m_params;
  Logger *const m_logger;

public:
  Experiment(Logger *log, Map *map, ExperimentParameters const &params)
      : m_level(map, 2), m_params(params), m_logger(log) {
    for (std::size_t i = 0; i < m_params.maximal_mice_number; i++) {
      m_mice.emplace_back(map);
    }
    start_of_the_round();
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
      bool outside_the_map = !mouse.advance(m_params, predators_states);
      if (outside_the_map) {
        mouse.add_to_fitness(-1);
      }
      for (std::unique_ptr<Predator> const &predator : m_level.m_predators) {
        if (predator->is_in_predator(mouse.get_position())) {
          mouse.add_to_fitness(-1);
        }
      }
      for (Food const &food : m_level.m_food) {
        if (food.can_eat(mouse.get_position())) {
          if (mouse.consumes(food.get_id())) {
            mouse.add_to_fitness(10);
          }
        }
      }
    }

    if (m_params.time >= m_params.generation_duration) {
      end_of_generation();
    }
  }

  void add_to_dt(double dt) { m_params.dt = std::max(0.005, m_params.dt + dt); }

private:
  void start_of_the_round() {
    m_params.generation++;
    m_params.time = 0.;

    for (Mouse &mouse : m_mice) {
      mouse.start_of_round();
      if (!m_params.randomized_spawning_point) {
        mouse.set_position(m_params.spawning_point);
        mouse.set_angle(m_params.spawning_angle);
      } else {
        mouse.randomize_position();
      }
    }
    for (std::unique_ptr<Predator> const &predator : m_level.m_predators) {
      predator->start_of_the_round();
    }
  }

  void end_of_generation() {
    send_to_logger();
    reproduce(m_mice);
    start_of_the_round();
  }

  void send_to_logger() {
    std::vector<double> fitness;
    std::vector<double> brain_connection;
    std::vector<double> brain_nodes;
    for (Mouse const &mouse : m_mice) {
      fitness.push_back(mouse.get_fitness());
      brain_connection.push_back(mouse.get_connections_number());
      brain_nodes.push_back(mouse.get_nodes_number());
    }
    m_logger->store(m_params.generation, fitness, brain_connection,
                    brain_nodes);
  }
};
