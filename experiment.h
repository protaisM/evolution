#pragma once

#include "brain.h"
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

template <typename Mouse, unsigned int MICE_NUMBER> class SelectionStrategy {
public:
  virtual ~SelectionStrategy() {}

  //---------------------------------------------------------------//
  // virtual functions, every subclass should implement these
  virtual void reproduce(std::array<Mouse, MICE_NUMBER> &,
                         ExperimentParameters &) = 0;
  //---------------------------------------------------------------//
};

template <typename Mouse, unsigned int MICE_NUMBER>
class OnlyReproduceAlive : public SelectionStrategy<Mouse, MICE_NUMBER> {
public:
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
      }
    }
  }
};

template <typename Mouse, unsigned int MICE_NUMBER> class Experiment {
private:
  Map *m_map;
  Logger *const m_logger;

  SelectionStrategy<Mouse, MICE_NUMBER> *m_reproduction_strat;

  // coming up: food?
  std::array<Mouse, MICE_NUMBER> m_mice;
  std::vector<Predator::Predator> m_predators;
  std::vector<Predator::Strategy *> m_predator_strategies;
  std::vector<Predator::Shape *> m_predator_shapes;

  // parameters
  ExperimentParameters m_params;
  DisplayParameters m_display_parameters;

public:
  Experiment(Logger *log, unsigned int minimal_mice_number = 750,
             int generation_duration = 20)
      : m_logger(log) {

    // map
    m_map = new Torus(1);

    // reproduction strategy
    m_reproduction_strat = new OnlyReproduceAlive<Mouse, MICE_NUMBER>();

    // predators
    Predator::Shape *predator_shape = new Predator::Circle(m_map, 0.1);
    m_predator_shapes.push_back(predator_shape);
    for (size_t i = 0; i < 10; i++) {
      Predator::Strategy *predator_strategy = new Predator::FollowPath(m_map);
      m_predator_strategies.push_back(predator_strategy);
      Predator::Predator predator(predator_shape, predator_strategy);
      m_predators.push_back(predator);
    }

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
    delete m_map;
    delete m_reproduction_strat;
    for (Predator::Shape *shape : m_predator_shapes) {
      delete shape;
    }
    for (Predator::Strategy *strat : m_predator_strategies) {
      delete strat;
    }
  }

  void add_predator(Predator::Predator predator) {
    m_predators.push_back(predator);
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
      bool should_die = !mouse.advance(m_params.dt, predators_states);
      for (Predator::Predator const &predator : m_predators) {
        if (predator.is_in_predator(mouse.get_position())) {
          should_die = true;
        }
      }
      if (should_die) {
        // the mouse fell outside of the map (that had no boundaries)
        // or has been eaten by a predator...
        mouse.kill();
        m_params.nb_alive_mice--;
      }
    }

    if (condition_end_of_generation()) {
      m_logger->store(m_params.generation,
                      m_params.time / m_params.generation_duration,
                      (double)m_params.nb_alive_mice / (double)MICE_NUMBER);
      m_reproduction_strat->reproduce(m_mice, m_params);
      for (Predator::Predator &predator : m_predators) {
        predator.start_of_the_round();
      }
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

private:
  bool condition_end_of_generation() {
    return (m_params.nb_alive_mice < m_params.minimal_mice_number) ||
           m_params.time >= m_params.generation_duration;
  }
};
