#pragma once

#include "experiment_parameter.h"
#include "food.h"

#include <array>
#include <iostream>

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
