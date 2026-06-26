#pragma once

#include "helper_brain.h"

struct Connection {
  double weight;
  unsigned int idx_node_in;
  unsigned int idx_node_out;
  bool enabled;

  Connection(unsigned int node_in, unsigned int node_out) {
    idx_node_in = node_in;
    idx_node_out = node_out;
    weight = 0.2 * (((double)rand() / RAND_MAX) * 2 - 1);
    enabled = true;
  }

  Connection(unsigned int node_in, unsigned int node_out, double w) {
    idx_node_in = node_in;
    idx_node_out = node_out;
    weight = w;
    enabled = true;
  }

  Connection() {}

  void mutate(double factor) { weight += factor * rand_normal(); }
  void toggle_enabled() { enabled = !enabled; }
};
