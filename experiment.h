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
#include <SFML/Window/Event.hpp>
#include <array>
#include <cstring>
#include <iostream>
#include <string>

enum Screen_type { FULL, ONLY_MAP, ONLY_LEGEND, EMPTY };

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
};

struct DisplayParameters {
  // this struct holds all parameters related to the
  // display of the experiment
  unsigned int selected_mouse;
  unsigned int zoom;
  Position center_position;
  bool follow_mouse;
};

template <typename Mouse, unsigned int MICE_NUMBER> class Experiment {
private:
  std::vector<Predator::BasePredator *> m_predators;
  std::array<Mouse, MICE_NUMBER> m_mice;
  Map *const m_map;
  Logger *const m_log;

  ExperimentParameters m_params;
  DisplayParameters m_display_parameters;

  // TODO: remove
  double m_map_display_size;
  char m_title[40];

public:
  Experiment(char title[40], Map *map, Logger *log,
             double mouse_sight_radius = 0.3,
             unsigned int minimal_mice_number = 100,
             int generation_duration = 200)
      : m_map(map), m_log(log) {
    strcpy(m_title, title);
    for (unsigned int i = 0; i < MICE_NUMBER; i++) {
      m_mice[i] = Mouse(m_map, mouse_sight_radius);
    }
    m_params.minimal_mice_number = minimal_mice_number;
    m_params.generation_duration = generation_duration;
    m_params.nb_alive_mice = MICE_NUMBER;
    m_params.dt = 0.005;
    m_params.generation = 0;
    m_params.time = 0;

    m_display_parameters.center_position = map->get_center();
    m_display_parameters.selected_mouse = 0;
    m_display_parameters.zoom = 1;
    m_display_parameters.follow_mouse = false;

    m_map_display_size = 940;
  }

  void add_predator(Predator::BasePredator *predator) {
    m_predators.push_back(predator);
  }

  void run_on_background() {
    m_log->store_begin(m_params.generation);
    while (true) {
      do_one_step();
    }
  }

  void run_and_display() {
    m_log->store_begin(m_params.generation);
    double space_right =
        sf::VideoMode::getDesktopMode().width - m_map_display_size;
    double space_bottom =
        sf::VideoMode::getDesktopMode().height - m_map_display_size;
    sf::RenderWindow window(sf::VideoMode(m_map_display_size + space_right,
                                          m_map_display_size + space_bottom),
                            std::string(m_title));
    run_on_window(&window);
  }

private:
  void do_one_step() {
    m_params.time = m_params.time + m_params.dt;
    std::vector<PositionAngle> predators_states;
    for (Predator::BasePredator *predator : m_predators) {
      predator->advance(m_params.dt);
      predators_states.push_back(predator->get_state());
    }
    for (Mouse &mouse : m_mice) {
      if (!mouse.is_alive()) {
        continue;
      }
      bool should_die = !mouse.advance(m_params.dt, predators_states);
      for (Predator::BasePredator *predator : m_predators) {
        if (predator->is_in_death_zone(mouse.get_position(),
                                       m_params.time /
                                           m_params.generation_duration)) {
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
      m_log->store_end(m_params.time / m_params.generation_duration,
                       (double)m_params.nb_alive_mice / (double)MICE_NUMBER);
      reproduction_round();
      for (Predator::BasePredator *predator : m_predators) {
        predator->clear_position();
      }
      m_params.generation++;
      m_params.time -= m_params.time;
      m_log->store_begin(m_params.generation);
    }
  }

  bool condition_end_of_generation() {
    return (m_params.nb_alive_mice < m_params.minimal_mice_number) ||
           m_params.time >= m_params.generation_duration;
  }

  void reproduction_round() {
    if (m_params.nb_alive_mice <= 0) {
      std::cout << "No mice to reproduce, everybody gets a second chance!"
                << std::endl;
      for (Mouse &mouse : m_mice) {
        mouse.resurrect();
      }
      m_params.nb_alive_mice = MICE_NUMBER;
      return;
    }
    unsigned int reproduction_rate = MICE_NUMBER / m_params.nb_alive_mice;
    unsigned int count_alive_mice = 0;
    unsigned int index_new_mouse = 0;
    std::array<Mouse, MICE_NUMBER> new_mice;
    for (unsigned int index_mouse_to_reproduce = 0;
         index_mouse_to_reproduce < MICE_NUMBER; index_mouse_to_reproduce++) {
      if (!m_mice[index_mouse_to_reproduce].is_alive()) {
        continue;
      }
      // each mouse can reproduce a certain number of time
      for (unsigned int baby = 0; baby < reproduction_rate; baby++) {
        index_new_mouse = count_alive_mice * reproduction_rate + baby;
        if (count_alive_mice >= m_params.nb_alive_mice) {
          std::cout << "You're still bad at counting" << std::endl;
          break;
        }
        if (index_new_mouse >= MICE_NUMBER) {
          std::cerr << "Error in the reproduction" << std::endl;
          break;
        }
        new_mice[index_new_mouse] = m_mice[index_mouse_to_reproduce];
        new_mice[index_new_mouse].mutate();
        new_mice[index_new_mouse].randomize_position();
      }
      count_alive_mice++;
    }
    m_params.nb_alive_mice = index_new_mouse + 1;
    // if reproduction rate was not the evolutive pressure,
    // m_mices is not full: we populate the space between
    // nb_alive_mice and MICE_NUMBER
    unsigned int mouse_to_copy;
    for (unsigned int index_new_mouse = m_params.nb_alive_mice;
         index_new_mouse < MICE_NUMBER; index_new_mouse++) {
      mouse_to_copy = rnd_int_smaller_than(m_params.nb_alive_mice);
      new_mice[index_new_mouse] = new_mice[mouse_to_copy];
      new_mice[index_new_mouse].mutate();
      new_mice[index_new_mouse].randomize_position();
    }
    m_mice = new_mice;
    m_params.nb_alive_mice = MICE_NUMBER;
  }

  void run_on_window(sf::RenderWindow *window) {
    // we start with full screen
    Screen_type display = FULL;
    double space_right =
        sf::VideoMode::getDesktopMode().width - m_map_display_size;
    double space_bottom =
        sf::VideoMode::getDesktopMode().height - m_map_display_size;
    while (window->isOpen()) {
      sf::Event evnt;
      while (window->pollEvent(evnt)) {
        handle_event(window, evnt, display);
      }
      do_one_step();
      window->clear(sf::Color({50, 50, 50}));
      draw(window, display, space_right, space_bottom);
    }
  }
  void handle_event(sf::RenderWindow *window, sf::Event evnt,
                    Screen_type &display) {
    switch (evnt.type) {
    case sf::Event::Closed: // close the window
      window->close();
      break;
    case (sf::Event::KeyReleased):
      if (evnt.key.code == sf::Keyboard::Num1) {
        display = FULL;
      }
      if (evnt.key.code == sf::Keyboard::Num2) {
        display = ONLY_MAP;
      }
      if (evnt.key.code == sf::Keyboard::Num3) {
        display = ONLY_LEGEND;
        window->clear();
      }
      if (evnt.key.code == sf::Keyboard::Num4) {
        display = EMPTY;
        window->clear();
      }
      if (evnt.key.code == sf::Keyboard::J) {
        m_params.dt -= 0.005;
        if (m_params.dt < 0) {
          m_params.dt = 0;
        }
      }
      if (evnt.key.code == sf::Keyboard::K) {
        m_params.dt += 0.005;
      }
      if (evnt.key.code == sf::Keyboard::Up) {
        m_params.generation_duration += 10;
      }
      if (evnt.key.code == sf::Keyboard::Down) {
        m_params.generation_duration -= 10;
        if (m_params.generation_duration < 10) {
          m_params.generation_duration = 10;
        }
      }
      if (evnt.key.code == sf::Keyboard::Right) {
        m_params.minimal_mice_number += 10;
      }
      if (evnt.key.code == sf::Keyboard::Left) {
        m_params.minimal_mice_number -= 10;
        if (m_params.minimal_mice_number < 10) {
          m_params.minimal_mice_number = 10;
        }
      }
      break;
    default:
      break;
    }
  }

  void draw(sf::RenderWindow *window, Screen_type display, int space_right,
            int space_bottom) const {
    switch (display) {
    case FULL:
      for (size_t i = 0; i < MICE_NUMBER; i++) {
        if (m_mice[i].is_alive()) {
          m_mice[i].draw(window,
                         m_display_parameters.zoom * m_map_display_size);
        }
      }
      for (Predator::BasePredator *predator : m_predators) {
        predator->draw(window, m_display_parameters.zoom * m_map_display_size);
      }
      m_map->draw(window, m_map_display_size);
      draw_legend(window, space_right, space_bottom);
      window->display();
      break;
    case ONLY_MAP:
      for (size_t i = 0; i < MICE_NUMBER; i++) {
        if (m_mice[i].is_alive()) {
          m_mice[i].draw(window,
                         m_display_parameters.zoom * m_map_display_size);
        }
      }
      for (Predator::BasePredator *predator : m_predators) {
        predator->draw(window, m_display_parameters.zoom * m_map_display_size);
      }
      m_map->draw(window, m_map_display_size);
      window->display();
      break;
    case ONLY_LEGEND:
      draw_legend(window, space_right, space_bottom);
      window->display();
      break;
    case EMPTY:
      window->clear();
    default:
      break;
    }
  }

  void draw_legend(sf::RenderWindow *window, int space_right,
                   int space_bottom) const {
    sf::Font font;
    font.loadFromFile("UbuntuMono-R.ttf");

    // legend at the bottom
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
    legend.setPosition(0.0f, 0.0f);
    legend.setCharacterSize(20);

    // the black boundary
    sf::RectangleShape boundary_right(
        sf::Vector2f(space_right, m_map_display_size + space_bottom));
    boundary_right.setPosition(m_map_display_size, 0.0f);
    boundary_right.setFillColor(sf::Color::Black);

    sf::RectangleShape boundary_down(
        sf::Vector2f(m_map_display_size, space_bottom));
    boundary_down.setPosition(0.0f, m_map_display_size);
    boundary_down.setFillColor(sf::Color::Black);

    window->draw(boundary_right);
    window->draw(boundary_down);
    window->draw(legend);
    // right panel
    m_log->plot(window, m_map_display_size + space_right, m_map_display_size,
                m_map_display_size + space_bottom);
  }
};
