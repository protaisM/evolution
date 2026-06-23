#pragma once

#include "brain.h"
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
#include <string>

struct ExperimentParameters {
  // this struct holds all the main parameters of an experiment
  // they are initialized in the beginning but they may change
  // during the simulation
  double time;
  unsigned int generation;
  double generation_duration;
  double dt;
  unsigned int nb_alive_mice;
  unsigned int minimal_mice_number;

  bool randomized_spawning_point;
  Position spawning_point;
  double spawning_angle;
};

struct DisplayParameters {
  // this struct holds all parameters related to the
  // display of the experiment
  unsigned int selected_mouse;
  unsigned int zoom;
  Position center_position;
  bool follow_mouse;
};

template <typename Mouse, unsigned int MICE_NUMBER> class ExperimentRules {
public:
  virtual ~ExperimentRules() {}

  //---------------------------------------------------------------//
  // virtual functions, every subclass should implement these
  virtual void if_in_predator(Mouse &, ExperimentParameters &) = 0;
  virtual void if_eat_food(Mouse &, Food const &) = 0;
  virtual void if_outside_map(Mouse &, ExperimentParameters &) = 0;
  virtual bool condition_end_generation(ExperimentParameters &) = 0;
  virtual void reproduce(std::array<Mouse, MICE_NUMBER> &,
                         ExperimentParameters &) = 0;
  //---------------------------------------------------------------//
};

template <typename Mouse, unsigned int MICE_NUMBER>
class KillingMice : public ExperimentRules<Mouse, MICE_NUMBER> {

public:
  void if_in_predator(Mouse &mouse, ExperimentParameters &params) override {
    if (mouse.kill()) {
      params.nb_alive_mice--;
    }
  }

  void if_eat_food(Mouse &mouse, Food const &food) override {
    if (mouse.has_consumed(food.get_id())) {
      return;
    }
    mouse.add_one_life(food.get_id());
  }

  void if_outside_map(Mouse &mouse, ExperimentParameters &params) override {
    if (mouse.kill()) {
      params.nb_alive_mice--;
    }
  }

  // we end the generation if we have no enough mice, or if the time is over
  bool condition_end_generation(ExperimentParameters &params) override {
    return (params.nb_alive_mice < params.minimal_mice_number) ||
           params.time >= params.generation_duration;
  }

  void reproduce(std::array<Mouse, MICE_NUMBER> &mice,
                 ExperimentParameters &params) override {
    // default
    params.generation++;
    params.time -= params.time;

    if (params.nb_alive_mice <= 0) {
      std::cout << "No mice to reproduce, everybody gets a second chance!"
                << std::endl;
      for (Mouse &mouse : mice) {
        mouse.resurrect();
        mouse.mutate();
      }
      params.nb_alive_mice = MICE_NUMBER;
    } else {
      unsigned int reproduction_rate = MICE_NUMBER / params.nb_alive_mice;
      unsigned int count_alive_mice = 0;
      unsigned int index_new_mouse = 0;
      std::array<Mouse, MICE_NUMBER> new_mice;
      for (unsigned int index_mouse_to_reproduce = 0;
           index_mouse_to_reproduce < MICE_NUMBER; index_mouse_to_reproduce++) {
        if (!mice[index_mouse_to_reproduce].is_alive()) {
          continue;
        }
        // each mouse can reproduce a certain number of time
        for (unsigned int baby = 0; baby < reproduction_rate; baby++) {
          index_new_mouse = count_alive_mice * reproduction_rate + baby;
          if (count_alive_mice >= params.nb_alive_mice) {
            std::cout << "You're still bad at counting" << std::endl;
            break;
          }
          if (index_new_mouse >= MICE_NUMBER) {
            std::cerr << "Error in the reproduction" << std::endl;
            break;
          }
          new_mice[index_new_mouse] = mice[index_mouse_to_reproduce];
          new_mice[index_new_mouse].mutate();
        }
        count_alive_mice++;
      }
      params.nb_alive_mice = index_new_mouse + 1;
      // if reproduction rate was not the evolutive pressure,
      // m_mices is not full: we populate the space between
      // nb_alive_mice and MICE_NUMBER
      unsigned int mouse_to_copy;
      for (unsigned int index_new_mouse = params.nb_alive_mice;
           index_new_mouse < MICE_NUMBER; index_new_mouse++) {
        mouse_to_copy = rnd_int_smaller_than(params.nb_alive_mice);
        new_mice[index_new_mouse] = new_mice[mouse_to_copy];
        new_mice[index_new_mouse].mutate();
      }
      mice = new_mice;
      params.nb_alive_mice = MICE_NUMBER;
    }

    // we respawn all the mice
    for (Mouse &mouse : mice) {
      if (params.randomized_spawning_point) {
        mouse.randomize_position();
      } else {
        mouse.set_position(params.spawning_point);
        mouse.set_angle(params.spawning_angle);
        mouse.reset_lifes();
      }
    }
  }
};

template <typename Mouse, unsigned int MICE_NUMBER> class Experiment {
private:
  Map *m_map;
  Logger *const m_logger;

  ExperimentRules<Mouse, MICE_NUMBER> *m_experiment_rules;
  Level m_level;

  std::array<Mouse, MICE_NUMBER> m_mice;
  std::vector<Predator::Predator> m_predators;
  std::vector<Food> m_food;

  // parameters
  ExperimentParameters m_params;
  DisplayParameters m_display_parameters;

public:
  Experiment(Logger *log, Map *map, unsigned int minimal_mice_number = 750,
             int generation_duration = 20)
      : m_map(map), m_logger(log), m_level(map) {

    // reproduction strategy
    m_experiment_rules = new KillingMice<Mouse, MICE_NUMBER>();

    m_predators = m_level.create_all_predators();

    // food
    for (unsigned int i = 0; i < 20; i++) {
      m_food.emplace_back(m_map, m_map->rnd_position());
    }

    // all the parameters
    m_params.minimal_mice_number = minimal_mice_number;
    m_params.generation_duration = generation_duration;
    m_params.nb_alive_mice = MICE_NUMBER;
    m_params.dt = 0.005;
    m_params.generation = 0;
    m_params.time = 0;
    m_params.randomized_spawning_point = false;
    m_params.spawning_point = {0.5, 0.5};
    m_params.spawning_angle = 0;

    m_display_parameters.center_position = m_map->get_center();
    m_display_parameters.selected_mouse = MICE_NUMBER - 1;
    m_display_parameters.zoom = 1;
    m_display_parameters.follow_mouse = false;

    // mice
    for (unsigned int i = 0; i < MICE_NUMBER; i++) {
      m_mice[i] = Mouse(m_map);
      if (!m_params.randomized_spawning_point) {
        m_mice[i].set_position(m_params.spawning_point);
        m_mice[i].set_angle(m_params.spawning_angle);
      }
    }
  }

  ~Experiment() {
    delete m_experiment_rules;
  }

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
    for (Predator::Predator const &predator : m_predators) {
      predator.draw(window, offset, m_display_parameters.zoom * map_size);
    }
    for (Food const &food : m_food) {
      food.draw(window, offset, m_display_parameters.zoom * map_size);
    }
    m_map->draw(window, offset, map_size);
  }

  void do_one_step() {
    m_params.time = m_params.time + m_params.dt;
    std::vector<PositionAngle> predators_states;
    for (Predator::Predator &predator : m_predators) {
      predator.do_one_step(m_params.dt);
      predators_states.push_back(predator.get_state());
    }
    for (Mouse &mouse : m_mice) {
      if (!mouse.is_alive()) {
        continue;
      }
      bool outside_the_map = !mouse.advance(m_params.dt, predators_states);
      if (outside_the_map) {
        m_experiment_rules->if_outside_map(mouse, m_params);
      }
      for (Predator::Predator const &predator : m_predators) {
        if (predator.is_in_predator(mouse.get_position())) {
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
    for (Predator::Predator &predator : m_predators) {
      predator.start_of_the_round();
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
    sf::Font font;
    font.loadFromFile("UbuntuMono-R.ttf");
    sf::Text legend;
    legend.setFont(font);
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
