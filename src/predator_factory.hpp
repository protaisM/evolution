#pragma once

#include "map.h"
#include "position.h"
#include "predator.h"

class PredatorFactory {

private:
  Map *m_map;

public:
  PredatorFactory(Map *map) : m_map(map) {}

  Predator::Predator newCirclePredator(double radius, Position begin,
                                       Position end) {
    Predator::Shape *predator_shape = new Predator::Circle(m_map, radius);
    Predator::Strategy *predator_strategy =
        new Predator::FollowPath(m_map, begin, end);
    Predator::Predator out(predator_shape, predator_strategy);
    return out;
  }

  Predator::Predator newSquarePredator(double length, Position begin,
                                       Position end) {
    Predator::Shape *predator_shape =
        new Predator::Rectangle(m_map, length, length);
    Predator::Strategy *predator_strategy =
        new Predator::FollowPath(m_map, begin, end);
    Predator::Predator out(predator_shape, predator_strategy);
    return out;
  }
};
