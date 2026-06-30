#pragma once

#include "position.h"

struct ExperimentParameters {
  // this struct holds all the main parameters of an experiment
  // they are initialized in the beginning but they may change
  // during the simulation
  double time;
  unsigned int generation;
  double generation_duration;
  double dt;
  unsigned int maximal_mice_number;

  bool randomized_spawning_point;
  Position spawning_point;
  double spawning_angle;
};
