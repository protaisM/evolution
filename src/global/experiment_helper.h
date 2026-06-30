#pragma once

#include "helper_brain.h"

#include <algorithm>
#include <vector>

template <typename Player>
Player tournament_selection(std::vector<Player> const &pool, std::size_t nb) {
  std::size_t best_player = rnd_int_smaller_than(pool.size());
  for (std::size_t j = 1; j < nb; j++) {
    std::size_t contender = rnd_int_smaller_than(pool.size());
    if (pool[contender].get_fitness() > pool[best_player].get_fitness()) {
      best_player = contender;
    }
  }
  Player selected_player = pool[best_player];
  return selected_player;
}

template <typename Player> void sort_by_fitness(std::vector<Player> &pool) {
  std::sort(pool.begin(), pool.end(), [](const Player &a, const Player &b) {
    return a.get_fitness() > b.get_fitness();
  });
}

template <typename Player>
void reproduce(std::vector<Player> &pool, double elitism = 0.1) {
  std::vector<Player> new_pool = pool;
  sort_by_fitness(new_pool);

  const std::size_t elites =
      std::max<std::size_t>(1, static_cast<std::size_t>(elitism * pool.size()));
  for (unsigned int i = 0; i < elites; i++) {
    pool[i] = new_pool[i];
  }
  for (unsigned int i = elites; i < pool.size(); i++) {
    pool[i] = tournament_selection(new_pool, 5);
    pool[i].mutate();
  }
}
