#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "map.h"

struct Predator {
    double x;
    double y;
    double radius;
    double speed = 1.0;

    void draw(sf::RenderWindow *window) const {
        sf::CircleShape death(radius);
        death.setPosition(x, y);
        death.setFillColor(sf::Color::Red);
        death.setOrigin(radius, radius);
        window->draw(death);
    }

    std::array<double, 2> get_next_position_toward(std::array<double, 2> pt) {
        double direction_x = pt[0] - x;
        double direction_y = pt[1] - y;
        double direction_x_norm =
            speed * direction_x /
            sqrt(direction_x * direction_x + direction_y * direction_y);
        double direction_y_norm =
            speed * direction_y /
            sqrt(direction_x * direction_x + direction_y * direction_y);
        double next_x = x + direction_x_norm;
        double next_y = y + direction_y_norm;
        return {next_x, next_y};
    }
};

class Chase {
private:
    // updating parameters
    Map m_map;
    Predator m_predator;
    Safe_zone m_safe_zone;
    unsigned int m_time;
    unsigned int m_day;

    // constant parameters
    unsigned int m_map_height;
    unsigned int m_map_width;
    char m_title[40];
    unsigned int m_nb_birds;
    unsigned int m_reproduction_rate;
    double m_mutation_size;
    bool m_randomized_positions;
    int m_duration_day;

    unsigned int m_size_brain;
    double m_speed_bird;

private:
    void move_predator();
    void move_safe_zone();
    bool condition_end_of_day();

    void save_brain_to_file(/*ostream ?*/);
    void save_current_state();
    void run_on_window(sf::RenderWindow*);

    void draw(sf::RenderWindow *,int,int,int) const;
    void draw_legend(sf::RenderWindow *,int,int) const;

    void do_one_step();

public:
    Chase(char title[40], double width=1200, double height=960,
          unsigned int nb_birds = 1000, unsigned int reproduction_rate = 4,
          double mutation_size = 0.1, unsigned int size_brain = 1,
          bool randomized_position = false, double size_predator = 50,
          double speed_predator = 1.0, double size_safe_zone = 150,
          double speed_bird = 1.3, double angle_bird=1.0, int duration_day=20000, bool periodic=false);

    Chase(char title[40], Brain, double width=1200, double height=960,
          unsigned int nb_birds = 1000, unsigned int reproduction_rate = 4,
          double mutation_size = 0.1, unsigned int size_brain = 1,
          bool randomized_position = false, double size_predator = 50,
          double speed_predator = 1.0, double size_safe_zone = 150,
          double speed_bird = 1.3, double angle_bird=1.0, int duration_day=20000, bool periodic=false);
    // Chase(std::string);

    void run_and_display();
    void run_on_background();
    void resume();

    void set_random_pos(bool);
};
