#pragma once

#include "map.h"
#include "position.h"
#include "predator.h"
#include <memory>

class PredatorFactory {

private:
  Map *m_map;

public:
  PredatorFactory(Map *map) : m_map(map) {}

  std::unique_ptr<Predator::Predator> newCirclePredator(double radius,
                                                        Position begin,
                                                        Position end,
                                                        double frequency) {
    std::unique_ptr<Predator::Shape> shape =
        std::make_unique<Predator::Circle>(m_map, radius);
    std::unique_ptr<Predator::Strategy> strategy =
        std::make_unique<Predator::FollowPath>(m_map, begin, end, frequency);

    return std::make_unique<Predator::Predator>(std::move(shape),
                                                std::move(strategy));
  }

  std::unique_ptr<Predator::Predator> newCirclePredator(double radius) {
    std::unique_ptr<Predator::Shape> shape =
        std::make_unique<Predator::Circle>(m_map, radius);
    std::unique_ptr<Predator::Strategy> strategy =
        std::make_unique<Predator::FollowPath>(m_map);

    return std::make_unique<Predator::Predator>(std::move(shape),
                                                std::move(strategy));
  }

  std::unique_ptr<Predator::Predator> newSquarePredator(double length,
                                                        Position begin,
                                                        Position end,
                                                        double frequency) {
    std::unique_ptr<Predator::Shape> shape =
        std::make_unique<Predator::Rectangle>(m_map, length, length);
    std::unique_ptr<Predator::Strategy> strategy =
        std::make_unique<Predator::FollowPath>(m_map, begin, end, frequency);

    return std::make_unique<Predator::Predator>(std::move(shape),
                                                std::move(strategy));
  }

  std::unique_ptr<Predator::Predator> newSquarePredator(double length) {
    std::unique_ptr<Predator::Shape> shape =
        std::make_unique<Predator::Rectangle>(m_map, length, length);
    std::unique_ptr<Predator::Strategy> strategy =
        std::make_unique<Predator::FollowPath>(m_map);

    return std::make_unique<Predator::Predator>(std::move(shape),
                                                std::move(strategy));
  }
};
