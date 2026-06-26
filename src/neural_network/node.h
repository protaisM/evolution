#pragma once

#include "helper_brain.h"
#include <cmath>

struct Node {
  double value;
  double bias;

  Node() : value(0), bias(0.01 * (((double)rand() / RAND_MAX) * 2 - 1)) {}
  Node(double b) : value(0), bias(b) {}

  void init() { value = 0; }
  double get_value() const { return value; }
  void set_value(double x) { value = x; }
  double add_to_value(double x) {
    value += x;
    return value;
  }
  double activate() const { return std::tanh(value + bias); }
  void mutate(double factor) { bias += 0.25 * factor * rand_normal(); }
};
