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

// struct Safe_zone {
//   double x;
//   double y;
//   double radius;
//
//   void draw(sf::RenderWindow *window) const {
//     sf::CircleShape safe(radius);
//     safe.setPosition(x, y);
//     safe.setFillColor(sf::Color(0, 255, 0, 100));
//     safe.setOrigin(radius, radius);
//     window->draw(safe);
//   }
//
//   bool is_in(std::array<double, 2>) const;
// };

template <typename Mouse, unsigned int MICE_NUMBER,
          unsigned int PREDATORS_NUMBER>
class Experiment {
private:
  std::array<Predator::BasePredator *, PREDATORS_NUMBER> m_predators;
  std::array<Mouse, MICE_NUMBER> m_mice;
  unsigned int m_nb_alive_mice;
  // Safe_zone m_safe_zone;

  double m_time;
  unsigned int m_generation;

  // constant parameters
  Map *m_map;
  Logger *m_log;
  char m_title[40];
  unsigned int m_evolutive_pressure;
  int m_duration_generation;
  double m_map_display_size;
  double m_zoom;
  double m_dt;

  // for display
  unsigned int m_mouse_selected = 0;

public:
  Experiment(const char title[40], Map *map,
             std::array<Predator::BasePredator *, PREDATORS_NUMBER> predators,
             Logger *log, double mouse_radius = 0.3,
             unsigned int evolutive_pressure = 4, int duration_generation = 200)
      : m_predators(predators), m_nb_alive_mice(MICE_NUMBER), m_time(0.0),
        m_generation(0), m_map(map), m_log(log),
        m_evolutive_pressure(evolutive_pressure),
        m_duration_generation(duration_generation), m_map_display_size(940),
        m_zoom(1.), m_dt(0.005) {
    strcpy(m_title, title);
    for (unsigned int i = 0; i < MICE_NUMBER; i++) {
      m_mice[i] = Mouse(m_map, mouse_radius);
    }
  }

  void run_on_background() {
    m_log->store_begin(m_generation);
    while (true) {
      do_one_step();
    }
  }

  void run_and_display() {
    m_log->store_begin(m_generation);
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
    m_time = m_time + m_dt;
    std::array<PositionAngle, PREDATORS_NUMBER> predators_states;
    for (unsigned int i = 0; i < PREDATORS_NUMBER; i++) {
      Predator::BasePredator *predator = m_predators[i];
      predator->advance(m_dt);
      predators_states[i] = predator->get_state();
    }
    for (Mouse &mouse : m_mice) {
      if (!mouse.is_alive()) {
        continue;
      }
      bool should_die = !mouse.advance(m_dt, predators_states);
      for (Predator::BasePredator *predator : m_predators) {
        if (predator->is_in_death_zone(mouse.get_position(),
                                       m_time / m_duration_generation)) {
          should_die = true;
        }
      }
      if (should_die) {
        // the mouse fell outside of the map (that had no boundaries)
        // or has been eaten by a predator...
        mouse.kill();
        m_nb_alive_mice--;
      }
    }

    if (condition_end_of_generation()) {
      m_log->store_end(m_time / m_duration_generation,
                       (double)m_nb_alive_mice / (double)MICE_NUMBER);
      reproduction_round();
      //   move_safe_zone();
      for (Predator::BasePredator *predator : m_predators) {
        predator->clear_position();
      }
      m_generation++;
      m_time -= m_time;
      m_log->store_begin(m_generation);
    }
  }

  bool condition_end_of_generation() {
    return (m_nb_alive_mice < MICE_NUMBER / m_evolutive_pressure + 1) ||
           m_time >= m_duration_generation;
  }

  void reproduction_round() {
    if (m_nb_alive_mice <= 0) {
      std::cerr << "No mice to reproduce, everybody gets a second chance!"
                << std::endl;
      for (Mouse &mouse : m_mice) {
        mouse.resurrect();
      }
      m_nb_alive_mice = MICE_NUMBER;
      return;
    }
    unsigned int reproduction_rate = MICE_NUMBER / m_nb_alive_mice;
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
        if (count_alive_mice >= m_nb_alive_mice) {
          // std::cerr << "Error in the count of alive mice" << std::endl;
          // std::cout << "count_alive_mice = " << count_alive_mice
          //           << " and m_nb_alive_mice = " << m_nb_alive_mice
          //           << std::endl;
          // std::cout << "index_new_mouse = " << index_new_mouse
          //           << ", reproduction rate = " << reproduction_rate
          //           << " and baby = " << baby << std::endl;
          // std::cout << "m_mice is:" << std::endl;
          // for (unsigned int i = 0; i < MICE_NUMBER; i++) {
          //   m_mice[i].print();
          // }
          // std::cout << "And new mice is:" << std::endl;
          // for (unsigned int i = 0; i < MICE_NUMBER; i++) {
          //   new_mice[i].print();
          // }
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
    m_nb_alive_mice = index_new_mouse + 1;
    // if reproduction rate was not the evolutive pressure,
    // m_mices is not full: we populate the space between
    // nb_alive_mice and MICE_NUMBER
    unsigned int mouse_to_copy;
    for (unsigned int index_new_mouse = m_nb_alive_mice;
         index_new_mouse < MICE_NUMBER; index_new_mouse++) {
      mouse_to_copy = rnd_int_smaller_than(m_nb_alive_mice);
      new_mice[index_new_mouse] = new_mice[mouse_to_copy];
      new_mice[index_new_mouse].mutate();
      new_mice[index_new_mouse].randomize_position();
    }
    m_mice = new_mice;
    m_nb_alive_mice = MICE_NUMBER;
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
      draw(window, display, space_right, space_bottom, m_dt);
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
        m_dt -= 0.005;
        if (m_dt < 0) {
          m_dt = 0;
        }
      }
      if (evnt.key.code == sf::Keyboard::K) {
        m_dt += 0.005;
      }
      break;
    default:
      break;
    }
  }

  void draw(sf::RenderWindow *window, Screen_type display, int space_right,
            int space_bottom, double dt) const {
    window->clear(sf::Color({50, 50, 50}));
    switch (display) {
    case FULL:
      for (size_t i = 0; i < MICE_NUMBER; i++) {
        if (m_mice[i].is_alive()) {
          m_mice[i].draw(window, m_zoom * m_map_display_size);
        }
      }
      for (Predator::BasePredator *predator : m_predators) {
        predator->draw(window, m_zoom * m_map_display_size);
      }
      m_map->draw(window, m_map_display_size);
      // m_safe_zone.draw(window);
      draw_legend(window, space_right, space_bottom, dt);
      window->display();
      break;
    case ONLY_MAP:
      for (size_t i = 0; i < MICE_NUMBER; i++) {
        if (m_mice[i].is_alive()) {
          m_mice[i].draw(window, m_zoom * m_map_display_size);
        }
      }
      for (Predator::BasePredator *predator : m_predators) {
        predator->draw(window, m_zoom * m_map_display_size);
      }
      m_map->draw(window, m_map_display_size);
      // m_safe_zone.draw(window);
      window->display();
      break;
    case ONLY_LEGEND:
      draw_legend(window, space_right, space_bottom, dt);
      window->display();
      break;
    case EMPTY:
      window->clear();
    default:
      break;
    }
  }

  void draw_legend(sf::RenderWindow *window, int space_right, int space_bottom,
                   double dt) const {
    sf::Font font;
    font.loadFromFile("UbuntuMono-R.ttf");

    // legend at the bottom
    sf::Text legend;
    legend.setFont(font);
    std::string text_legend =
        " Mice alive: " + std::to_string(m_nb_alive_mice) + " / " +
        std::to_string(MICE_NUMBER) + ", ";
    text_legend += "in generation " + std::to_string(m_generation) +
                   " at time " + std::to_string(m_time) + " / " +
                   std::to_string(m_duration_generation) + "\n";
    text_legend +=
        " Evolutive pressure: " + std::to_string(m_evolutive_pressure) + ", ";
    text_legend +=
        "sight radius: " + std::to_string(m_mice[0].get_sight_radius()) + "\n";
    text_legend += " dt = " + std::to_string(dt);
    legend.setString(text_legend);
    legend.setFillColor(sf::Color::White);
    legend.setPosition(0.0f, m_map_display_size);
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
