#pragma once

#include "map.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <array>
#include <cstring>

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

template <typename Mouse, typename Predator, typename Map,
          unsigned int MICE_NUMBER>
class Experiment {
private:
  Predator m_predator;
  std::array<Mouse, MICE_NUMBER> m_mices;
  unsigned int m_nb_alive_mices;
  // Safe_zone m_safe_zone;

  double m_time;
  unsigned int m_day;

  // constant parameters
  const Map m_map;
  char m_title[40];
  const unsigned int m_evolutive_pressure;
  const double m_mutation_strength;
  const int m_duration_day;
  const double m_window_size = 960;

public:
  Experiment(char title[40], Map map, double predator_radius = 0.01,
             unsigned int evolutive_pressure = 4,
             double mutation_strength = 0.1, int duration_day = 20000)
      : m_evolutive_pressure(evolutive_pressure),
        m_mutation_strength(mutation_strength), m_duration_day(duration_day),
        m_nb_alive_mices(MICE_NUMBER), m_map(map), m_time(0.0), m_day(0) {
    strcpy(m_title, title);
    for (unsigned int i = 0; i < MICE_NUMBER; i++) {
      m_mices[i] = Mouse([&map]() { return map.rnd_position(); });
    }
    m_predator =
        Predator([&map]() { return map.rnd_position(); }, predator_radius);
  }

  void run_on_background(double dt) {
    while (true) {
      do_one_step(dt);
    }
  }

  void run_and_display(double dt) {
    double space_right = sf::VideoMode::getDesktopMode().width - m_window_size;
    double space_bottom =
        sf::VideoMode::getDesktopMode().height - m_window_size;
    sf::RenderWindow window(sf::VideoMode(m_window_size + space_right,
                                          m_window_size + space_bottom),
                            std::string(m_title));
    run_on_window(&window, dt);
  }

private:
  void do_one_step(double dt) {
    m_time = m_time + dt;
    for (unsigned int i = 0; i < MICE_NUMBER; i++) {
      if (!m_mices[i].is_alive()) {
        continue;
      }
      m_mices[i].advance(dt, m_predator.get_position(),
                         [this](Position pos) { return m_map.is_in(pos); });
    }
    m_predator.advance(dt, [this](Position pos) { return m_map.is_in(pos); });
    // m_map.kill_birds_in_circle(m_predator.x, m_predator.y,
    // m_predator.radius); m_time++; if (condition_end_of_day()) {
    //   // std::cout << "end of the day !"<< std::endl;
    //   m_map.reproduce_alive(m_mutation_size, m_randomized_positions);
    //   move_safe_zone();
    //   m_day++;
    //   m_time -= m_time;
    //   save_current_state();
    // }
  }

  // void save_brain_to_file(/*ostream ?*/);
  // void save_current_state();
  // void run_on_window(sf::RenderWindow *);

  // void draw(sf::RenderWindow *, int, int, int) const;
  // void draw_legend(sf::RenderWindow *, int, int) const;

  void run_on_window(sf::RenderWindow *window, double dt) {
    int screen = 1;
    double space_right = sf::VideoMode::getDesktopMode().width - m_window_size;
    double space_bottom =
        sf::VideoMode::getDesktopMode().height - m_window_size;
    while (window->isOpen()) {
      sf::Event evnt;
      while (window->pollEvent(evnt)) {
        handle_event(window, evnt, screen);
      }
      do_one_step(dt);
      draw(window, screen, space_right, space_bottom);
    }
  }
  void handle_event(sf::RenderWindow *window, sf::Event evnt, int &screen) {
    switch (evnt.type) {
    case sf::Event::Closed: // close the window
      window->close();
      break;
    case (sf::Event::KeyReleased):
      if (evnt.key.code == sf::Keyboard::Num1) // display screen 1
      {
        screen = 1;
      }
      if (evnt.key.code == sf::Keyboard::Num2) // display screen 2 (no legend)
      {
        screen = 2;
      }
      if (evnt.key.code == sf::Keyboard::Num3) // display screen 3 (only legend)
      {
        screen = 3;
        window->clear();
      }
      if (evnt.key.code == sf::Keyboard::Num4) // display screen 4 (nothing)
      {
        screen = 4;
        window->clear();
      }
      break;
    default:
      break;
    }
  }

  void draw(sf::RenderWindow *window, int screen, int space_right,
            int space_bottom) const {
    window->clear();
    switch (screen) {
    case 1:
      for (size_t i = 0; i < MICE_NUMBER; i++) {
        if (m_mices[i].is_alive()) {
          m_mices[i].draw(window, m_window_size);
        }
      }
      m_predator.draw(window, m_window_size);
      // m_safe_zone.draw(window);
      draw_legend(window, space_right, space_bottom);
      window->display();
      break;
    case 2:
      for (size_t i = 0; i < MICE_NUMBER; i++) {
        if (m_mices[i].is_alive()) {
          m_mices[i].draw(window, m_window_size);
        }
      }
      m_predator.draw(window, m_window_size);
      // m_safe_zone.draw(window);
      window->display();
      break;
    case 3:
      draw_legend(window, space_right, space_bottom);
      window->display();
      break;
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
    std::string text_legend =
        " Mices alive : " + std::to_string(m_nb_alive_mices) + "\n";
    text_legend +=
        " Day " + std::to_string(m_day) + " and time " + std::to_string(m_time);
    legend.setString(text_legend);
    legend.setFillColor(sf::Color::White);
    legend.setPosition(0.0f, m_window_size);
    legend.setCharacterSize(20);

    // the right panel
    sf::Text panel;
    panel.setFont(font);
    std::string text_panel = "Parameters of the experiment : \n";
    text_panel += "Title : " + std::string(m_title) + "\n";
    text_panel += "Zoom : (" + std::to_string(m_window_size) + "," +
                  std::to_string(m_window_size) + ")\n";
    text_panel += "Max number of mices : " + std::to_string(MICE_NUMBER) + "\n";
    text_panel +=
        "Duration of the day : " + std::to_string(m_duration_day) + "\n";
    text_panel +=
        "Evolutive pressure : " + std::to_string(m_evolutive_pressure) + "\n";
    text_panel +=
        "Mutation strength : " + std::to_string(m_mutation_strength) + "\n";
    // text_panel +=
    //     "Size of the safe zone : " + std::to_string(m_safe_zone.radius) +
    //     "\n";
    text_panel +=
        "Size of the predator : " + std::to_string(m_predator.m_radius) + "\n";
    text_panel +=
        "Speed of the predator : " + std::to_string(m_predator.m_velocity) +
        "\n";
    text_panel += "\n";
    text_panel += "Day : " + std::to_string(m_day) +
                  ", Time :  " + std::to_string(m_time) + "\n";
    text_panel +=
        "Remaining mices : " + std::to_string(m_nb_alive_mices) + "\n";

    panel.setString(text_panel);
    panel.setFillColor(sf::Color::White);
    panel.setPosition(m_window_size + 10, 0.0f);
    panel.setCharacterSize(20);

    // the black boundary
    sf::RectangleShape boundary_right(
        sf::Vector2f(space_right, m_window_size + space_bottom));
    boundary_right.setPosition(m_window_size, 0.0f);
    boundary_right.setFillColor(sf::Color::Black);

    sf::RectangleShape boundary_down(sf::Vector2f(m_window_size, space_bottom));
    boundary_down.setPosition(0.0f, m_window_size);
    boundary_down.setFillColor(sf::Color::Black);

    // the lines around the map
    sf::Vertex line_bottom[] = {
        sf::Vertex(sf::Vector2f(0.0f, m_window_size)),
        sf::Vertex(sf::Vector2f(m_window_size, m_window_size))};

    sf::Vertex line_right[] = {
        sf::Vertex(sf::Vector2f(m_window_size, 0.0f)),
        sf::Vertex(sf::Vector2f(m_window_size, m_window_size))};
    window->draw(boundary_right);
    window->draw(boundary_down);
    window->draw(line_bottom, 2, sf::Lines);
    window->draw(line_right, 2, sf::Lines);

    window->draw(panel);
    window->draw(legend);
  }

  void resume();

  void set_random_pos(bool);
};
