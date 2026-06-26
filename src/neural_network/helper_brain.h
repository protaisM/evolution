#pragma once

#include <stdexcept>

inline unsigned int rnd_int_smaller_than(unsigned int bound) {
  if (bound <= 0) {
    throw std::runtime_error(
        "Seems like the bound in rnd_int_smaller_than is <= 0");
  }
  return rand() % bound;
}

inline double rand_0_1() { return (double)std::rand() / ((double)RAND_MAX); }

inline double rand_normal() {
  double result = -6;
  for (unsigned int i = 0; i < 12; i++) {
    result += rand_0_1();
  }
  result /= 6;
  return result;
}

inline double relu(double x) { return std::max(x, 0.0); }
