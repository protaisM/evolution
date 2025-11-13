#pragma once

// #include <SFML/Graphics.hpp>
#include <array>
#include <cmath>

struct Position {
  double x;
  double y;
};

class Map {
public:
  virtual bool is_in(Position) const = 0;
  virtual Position rnd_position() const = 0;
  virtual double distance(Position, Position) const = 0;
};

class Square : public Map {
private:
  double m_side_length;

public:
  Square(double side_length) : m_side_length(side_length) {}

  virtual bool is_in(Position pos) const {
    if (pos.x < 0 or pos.x > m_side_length or pos.y < 0 or
        pos.y > m_side_length) {
      return false;
    }
    return true;
  }

  virtual Position rnd_position() const {
    Position result;
    result.x = ((double)std::rand() / ((double)RAND_MAX)) * m_side_length;
    result.y = ((double)std::rand() / ((double)RAND_MAX)) * m_side_length;
    return result;
  }
  virtual double distance(Position p1, Position p2) const {}
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
