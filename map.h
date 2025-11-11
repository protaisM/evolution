#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>

#include "mouse.h"

struct Safe_zone {
  double x;
  double y;
  double radius;

  void draw(sf::RenderWindow *window) const {
    sf::CircleShape safe(radius);
    safe.setPosition(x, y);
    safe.setFillColor(sf::Color(0, 255, 0, 100));
    safe.setOrigin(radius, radius);
    window->draw(safe);
  }

  bool is_in(std::array<double, 2>) const;
};

template <unsigned int MAX_MICES_NUMBER, typename Mouse> class Map {
protected:
  std::array<Mouse, MAX_MICES_NUMBER> m_mices;
  unsigned int m_nb_mices;
  unsigned int m_nb_alive_mices;

public:
  bool is_in_map(std::array<double, 2> position) const;
  void do_one_step(std::array<double, 4>, double dt);
  void reproduce_alive(double, bool);
  void kill_birds_in_circle(double, double, double);
  std::array<double, 2> get_nearest_alive_bird(std::array<double, 2>) const;
  std::array<double, 2> get_nearest_alive_bird(std::array<double, 2>,
                                               Safe_zone) const;

  unsigned int get_nb_alive_birds() const;

  void draw(sf::RenderWindow *) const;
};
/*

const double SPEED_DEATH=1.1;
const double DEATH_RADIUS=45;
const double SAFE_RADIUS=150;
const int NB_REPRODUCTIONS=4;
const int THRESHOLD=1000;

class Map{
    protected:
    std::vector<Bird> birds;
    //add safe zone and death zone
    double x_safe,y_safe,x_death,y_death,angle_death;
    Brain brain_death_zone;

    public:
    virtual bool is_in(double,double) const=0;
    virtual void add_rnd_birds(int) =0;
    std::vector<Bird> get_birds();
    int get_nb_birds();
    double getDeathX();
    double getDeathY();
    double getSafeX();
    double getSafeY();
    virtual void do_one_step()=0;
};

class Rectangle : public Map {
    protected:
    double a; double b;

    public:
    Rectangle(double,double);

    virtual bool is_in(double,double) const;
    virtual void add_rnd_birds(int);
    void reproduce();
    void double_birds(int);
    virtual void do_one_step();
    void save(std::string);
    void read(std::string);


};
*/
