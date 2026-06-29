#pragma once

#include "experiment_parameter.h"
#include "food.h"
#include "helper_brain.h"

#include <algorithm>
#include <array>
#include <iostream>

template <std::size_t size>
unsigned int get_corresponding_index(double rnd,
                                     std::array<double, size> norm_fitness) {
  std::cout << "rnd = " << rnd << std::endl;
  unsigned int result = 0;
  double sum = norm_fitness[0];
  while (rnd > sum) {
    result++;
    sum += norm_fitness[result];
  }
  return result;
}

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

template <typename Mouse, unsigned int MICE_NUMBER>
class FitnessFunction : public ExperimentRules<Mouse, MICE_NUMBER> {

public:
  void if_in_predator(Mouse &mouse,
                      ExperimentParameters & /*params*/) override {
    mouse.add_to_fitness(-1);
  }

  void if_eat_food(Mouse &mouse, Food const &food) override {
    if (mouse.consumes(food.get_id())) {
      mouse.add_to_fitness(10);
    }
  }

  void if_outside_map(Mouse &mouse,
                      ExperimentParameters & /*params*/) override {
    mouse.add_to_fitness(-1);
  }

  // we end the generation if we have no enough mice, or if the time is over
  bool condition_end_generation(ExperimentParameters &params) override {
    return (params.time >= params.generation_duration);
  }

  void reproduce(std::array<Mouse, MICE_NUMBER> &mice,
                 ExperimentParameters &params) override {
    // default
    params.generation++;
    params.time = 0.;

    std::array<Mouse, MICE_NUMBER> old_mice = mice;
    // sort by fitness
    std::sort(old_mice.begin(), old_mice.end(),
              [](const Mouse &a, const Mouse &b) {
                return a.get_fitness() > b.get_fitness();
              });

    const double elitism = 0.1;
    const std::size_t elites = std::max<std::size_t>(
        1, static_cast<std::size_t>(elitism * MICE_NUMBER));

    for (unsigned int i = 0; i < elites; i++) {
      mice[i] = old_mice[i];
    }

    // tournament selection
    for (unsigned int i = elites; i < MICE_NUMBER; i++) {
      unsigned int best_mouse = rnd_int_smaller_than(mice.size());
      for (unsigned int j = 0; j < 4; j++) {
        unsigned int contender = rnd_int_smaller_than(mice.size());
        if (old_mice[contender].get_fitness() >
            old_mice[best_mouse].get_fitness()) {
          best_mouse = contender;
        }
      }
      mice[i] = old_mice[best_mouse];
      mice[i].mutate();
    }

    for (Mouse &mouse : mice) {
      if (params.randomized_spawning_point) {
        mouse.randomize_position();
      } else {
        mouse.set_position(params.spawning_point);
        mouse.set_angle(params.spawning_angle);
        mouse.resurrect();
      }
    }
  }
};
