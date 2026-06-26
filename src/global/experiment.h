#pragma once

#include "display_parameter.h"
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

template <typename Mouse, unsigned int MICE_NUMBER> class Experiment {
private:
  Map *m_map;
  Logger *const m_logger;

  std::unique_ptr<ExperimentRules<Mouse, MICE_NUMBER>> m_experiment_rules;
  Level m_level;

  std::array<Mouse, MICE_NUMBER> m_mice;
  std::vector<Food> m_food;

  // parameters
  ExperimentParameters m_params;
  DisplayParameters m_display_parameters;

public:
  Experiment(Logger *log, Map *map, ExperimentParameters const &params,
             DisplayParameters display_params)
      : m_map(map), m_logger(log), m_params(params),
        m_display_parameters(display_params), m_level(map, 1) {

    // reproduction strategy
    m_experiment_rules = std::make_unique<KillingMice<Mouse, MICE_NUMBER>>();

    // mice
    for (unsigned int i = 0; i < MICE_NUMBER; i++) {
      m_mice[i] = Mouse(m_map);
      if (!m_params.randomized_spawning_point) {
        m_mice[i].set_position(m_params.spawning_point);
        m_mice[i].set_angle(m_params.spawning_angle);
      }
    }
  }

  ~Experiment() = default;

  void add_food() { m_food.emplace_back(m_map, m_map->rnd_position()); }
  void add_food(Position position) { m_food.emplace_back(m_map, position); }

  void draw(sf::RenderWindow *window, sf::Vector2f offset,
            float map_size) const {
    sf::RectangleShape background(sf::Vector2f({map_size, map_size}));
    background.setPosition(offset);
    background.setFillColor(sf::Color({80, 80, 80}));
    window->draw(background);
    for (size_t idx_mouse = 0; idx_mouse < MICE_NUMBER; idx_mouse++) {
      if (m_mice[idx_mouse].is_alive()) {
        bool is_selected_mouse =
            (m_display_parameters.selected_mouse == idx_mouse);
        m_mice[idx_mouse].draw(window, offset,
                               m_display_parameters.zoom * map_size,
                               is_selected_mouse);
      }
    }
    for (std::unique_ptr<Predator::Predator> const &predator :
         m_level.m_predators) {
      predator->draw(window, offset, m_display_parameters.zoom * map_size);
    }
    for (Food const &food : m_food) {
      food.draw(window, offset, m_display_parameters.zoom * map_size);
    }
    m_map->draw(window, offset, map_size);
  }

  void do_one_step() {
    m_params.time = m_params.time + m_params.dt;
    std::vector<PositionAngle> predators_states;
    for (std::unique_ptr<Predator::Predator> const &predator :
         m_level.m_predators) {
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
      for (std::unique_ptr<Predator::Predator> const &predator :
           m_level.m_predators) {
        if (predator->is_in_predator(mouse.get_position())) {
          m_experiment_rules->if_in_predator(mouse, m_params);
        }
      }
      for (Food const &food : m_food) {
        if (food.can_eat(mouse.get_position())) {
          m_experiment_rules->if_eat_food(mouse, food);
        }
      }
    }

    if (!m_experiment_rules->condition_end_generation(m_params)) {
      return;
    }
    m_logger->store(m_params.generation,
                    m_params.time / m_params.generation_duration,
                    (double)m_params.nb_alive_mice / (double)MICE_NUMBER);
    m_experiment_rules->reproduce(m_mice, m_params);
    for (std::unique_ptr<Predator::Predator> const &predator :
         m_level.m_predators) {
      predator->start_of_the_round();
    }
  }

  void add_to_dt(double dt) { m_params.dt = std::max(0.005, m_params.dt + dt); }

  void add_to_generation_duration(double dg) {
    m_params.generation_duration =
        std::max(10., m_params.generation_duration + dg);
  }

  void add_to_minimal_mice_number(double dn) {
    m_params.minimal_mice_number =
        std::max(10., m_params.minimal_mice_number + dn);
  }

  void draw_legend(sf::RenderWindow *window, sf::Vector2f offset) const {
    sf::Text legend;
    legend.setFont(m_display_parameters.font);
    std::string text_legend = " Gen " + std::to_string(m_params.generation) +
                              " Mice " +
                              std::to_string(m_params.nb_alive_mice) + " (" +
                              std::to_string(m_params.minimal_mice_number) +
                              ") / " + std::to_string(MICE_NUMBER) + "\n";
    text_legend += " Time " + std::to_string(m_params.time) + " (+" +
                   std::to_string(m_params.dt) + ") / " +
                   std::to_string(m_params.generation_duration) + "\n";
    legend.setString(text_legend);
    legend.setFillColor(sf::Color::White);
    legend.setPosition(offset);
    legend.setCharacterSize(20);
    window->draw(legend);
  }
};
