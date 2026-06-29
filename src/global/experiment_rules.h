#pragma once

#include "experiment_parameter.h"
#include "food.h"
#include "helper_brain.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

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

template <typename Mouse> class ExperimentRules {
public:
  virtual ~ExperimentRules() {}

  //---------------------------------------------------------------//
  // virtual functions, every subclass should implement these
  virtual void if_in_predator(Mouse &, ExperimentParameters &) = 0;
  virtual void if_eat_food(Mouse &, Food const &) = 0;
  virtual void if_outside_map(Mouse &, ExperimentParameters &) = 0;
  virtual bool condition_end_generation(ExperimentParameters &) = 0;
  virtual void reproduce(std::vector<Mouse> &, ExperimentParameters &) = 0;
  //---------------------------------------------------------------//
};

template <typename Mouse>
class FitnessFunction : public ExperimentRules<Mouse> {

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

  void reproduce(std::vector<Mouse> &mice,
                 ExperimentParameters &params) override {
    // default
    params.generation++;
    params.time = 0.;

    std::vector<Mouse> old_mice = mice;
    // sort by fitness
    std::sort(old_mice.begin(), old_mice.end(),
              [](const Mouse &a, const Mouse &b) {
                return a.get_fitness() > b.get_fitness();
              });

    const double elitism = 0.1;
    const std::size_t elites = std::max<std::size_t>(
        1, static_cast<std::size_t>(elitism * mice.size()));

    for (unsigned int i = 0; i < elites; i++) {
      mice[i] = old_mice[i];
    }

    // tournament selection
    for (unsigned int i = elites; i < mice.size(); i++) {
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
