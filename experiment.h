#pragma once

// #include <SFML/Graphics.hpp>
// #include <SFML/Graphics/RenderWindow.hpp>
#include "map.h"

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

struct Predator {
  Position position;
  double radius;
  double velocity = 1.0;
  double angle;

  Position get_position() { return position; }

  // void draw(sf::RenderWindow *window) const {
  //   sf::CircleShape death(radius);
  //   death.setPosition(x, y);
  //   death.setFillColor(sf::Color::Red);
  //   death.setOrigin(radius, radius);
  //   window->draw(death);
};

//   std::array<double, 2> get_next_position_toward(std::array<double, 2> pt)
//   {
//     double direction_x = pt[0] - x;
//     double direction_y = pt[1] - y;
//     double direction_x_norm =
//         speed * direction_x /
//         sqrt(direction_x * direction_x + direction_y * direction_y);
//     double direction_y_norm =
//         speed * direction_y /
//         sqrt(direction_x * direction_x + direction_y * direction_y);
//     double next_x = x + direction_x_norm;
//     double next_y = y + direction_y_norm;
//     return {next_x, next_y};
//   }
// };

template <typename Mouse, typename Predator, typename Map,
          unsigned int MICE_NUMBER>
class Experiment {
private:
  Predator m_predator;
  std::array<Mouse, MICE_NUMBER> m_mices;
  unsigned int m_nb_alive_mices;
  // Safe_zone m_safe_zone;

  unsigned int m_time;
  unsigned int m_day;

  // constant parameters
  const Map m_map;
  char m_title[40];
  const unsigned int m_evolutive_pressure;
  const double m_mutation_strength;
  const int m_duration_day;

public:
  Experiment(char title[40], Map map, unsigned int evolutive_pressure = 4,
             double mutation_strength = 0.1, int duration_day = 20000)
      : m_evolutive_pressure(evolutive_pressure),
        m_mutation_strength(mutation_strength), m_duration_day(duration_day),
        m_nb_alive_mices(MICE_NUMBER), m_map(map), m_time(0), m_day(0) {
    strcpy(m_title, title);
    for (unsigned int i = 0; i < MICE_NUMBER; i++) {
      m_mices[i] = Mouse([&map]() { return map.rnd_position(); }, 1, 1);
    }
    // predator
  }

  void run_on_background(double dt) {
    while (true) {
      do_one_step(dt);
    }
  }


private:

  void do_one_step(double dt) {
    for (unsigned int i = 0; i < MICE_NUMBER; i++) {
      if (!m_mices[i].is_alive()) {
        continue;
      }
      m_mices[i].advance(dt, m_predator.get_position(),
                         [this](Position pos) { return m_map.is_in(pos); });
    }
    // move_predator();
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
  // void move_predator();
  // void move_safe_zone();
  // bool condition_end_of_day();

  // void save_brain_to_file(/*ostream ?*/);
  // void save_current_state();
  // void run_on_window(sf::RenderWindow *);

  // void draw(sf::RenderWindow *, int, int, int) const;
  // void draw_legend(sf::RenderWindow *, int, int) const;

  // void run_and_display() {
  //   double space_right = sf::VideoMode::getDesktopMode().width -
  //   m_map_width; double space_bottom =
  //   sf::VideoMode::getDesktopMode().height - m_map_height; sf::RenderWindow
  //   window(
  //       sf::VideoMode(m_map_width + space_right, m_map_height +
  //       space_bottom), std::string(m_title));
  //   run_on_window(&window);
  // }


  void resume();

  void set_random_pos(bool);
};
