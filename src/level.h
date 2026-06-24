#pragma once

#include <vector>

#include "map.h"
#include "predator.h"
#include "predator_factory.hpp"

struct Strat_Shape_Pair {
  unsigned int strat;
  unsigned int shape;
};

class Level {
public:
  std::vector<std::unique_ptr<Predator::Predator>> m_predators;

  Level(Map *map, unsigned int level_nb = 0) {
    PredatorFactory factory(map);
    switch (level_nb) {
    case 1: {
      // first, the square
      m_predators.push_back(
          factory.newSquarePredator(0.2, {0.3, 0.5}, {0.7, 0.5}, 3));

      // then, all the circles
      m_predators.push_back(
          factory.newCirclePredator(0.1, {0.15, 0.15}, {0.15, 0.85}, 5));
      m_predators.push_back(
          factory.newCirclePredator(0.1, {0.15, 0.85}, {0.85, 0.85}, 5));
      m_predators.push_back(
          factory.newCirclePredator(0.1, {0.85, 0.85}, {0.85, 0.15}, 5));
      m_predators.push_back(
          factory.newCirclePredator(0.1, {0.85, 0.15}, {0.15, 0.15}, 5));
      break;
    }
    default: {
      for (unsigned int i = 0; i < 5; i++) {
        m_predators.push_back(factory.newCirclePredator(0.1));
      }
      break;
    }
    }
  }

  ~Level() = default;
};
