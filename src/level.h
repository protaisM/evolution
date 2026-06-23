#pragma once

#include <vector>

#include "map.h"
#include "predator.h"

struct Pair {
  unsigned int strat;
  unsigned int shape;
};

class Level {

private:
  std::vector<Predator::Strategy *> m_predator_strategies;
  std::vector<Predator::Shape *> m_predator_shapes;

  std::vector<Pair> m_predators;

public:
  Level(Map *map) {
    Predator::Shape *predator_shape = new Predator::Circle(map, 0.1);
    m_predator_shapes.push_back(predator_shape);
    Pair pair{};
    pair.shape = 0;
    for (unsigned int i = 0; i < 5; i++) {
      Predator::Strategy *predator_strategy = new Predator::FollowPath(map);
      m_predator_strategies.push_back(predator_strategy);
      pair.strat = i;
      m_predators.push_back(pair);
    }
  }

  ~Level() {
    for (Predator::Shape *shape : m_predator_shapes) {
      delete shape;
    }
    for (Predator::Strategy *strat : m_predator_strategies) {
      delete strat;
    }
  }

  std::vector<Predator::Predator> create_all_predators() const {
    std::vector<Predator::Predator> predators;
    for (Pair pred_pair : m_predators) {
      predators.emplace_back(m_predator_shapes[pred_pair.shape],
                             m_predator_strategies[pred_pair.strat]);
    }
    return predators;
  }
};
