#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "connection.h"
#include "helper_brain.h"
#include "node.h"

template <unsigned int NB_IN_NODES, unsigned int NB_OUT_NODES> class Brain {
private:
  std::vector<Node> m_nodes;
  std::vector<Connection> m_connections;
  unsigned int m_nb_hidden_nodes;
  unsigned int m_nb_connections;

public:
  Brain(unsigned int nb_hidden_nodes = 0) : m_nb_hidden_nodes(nb_hidden_nodes) {
    /* The node layout is the following:
     * m_nodes = [ input_nodes, output_nodes, hidden_nodes ]
     */
    unsigned int total_nb_nodes =
        NB_IN_NODES + NB_OUT_NODES + m_nb_hidden_nodes;
    for (unsigned int i = 0; i < total_nb_nodes; i++) {
      m_nodes.emplace_back(); // default constructor
    }

    // create all connections, one for each output node
    m_nb_connections = NB_OUT_NODES;
    for (unsigned int i = 0; i < NB_OUT_NODES; i++) {
      unsigned int rnd_node_idx =
          rnd_int_smaller_than(NB_IN_NODES + m_nb_hidden_nodes);
      if (rnd_node_idx >= NB_IN_NODES) { // skip the out nodes
        rnd_node_idx += NB_OUT_NODES;
      }
      m_connections.emplace_back(rnd_node_idx, NB_IN_NODES + i);
    }
  }

  std::array<double, NB_OUT_NODES>
  activate(std::array<double, NB_IN_NODES> input) {
    set_to_zero();
    store_input(input);
    activate_all_connections();
    activate_last_layer();
    return extract_output();
  }

  void print() { std::cout << informations() << std::endl; }

  std::string informations() const {
    std::string result;
    result += "Nodes:\n";
    for (unsigned int i = 0; i < NB_IN_NODES; i++) {
      result += "(" + std::to_string(m_nodes[i].get_value()) + ")";
    }
    result += "\n";
    unsigned int hidden_node_beginning = NB_IN_NODES + NB_OUT_NODES;
    for (unsigned int i = hidden_node_beginning;
         i < hidden_node_beginning + m_nb_hidden_nodes; i++) {
      result += "(" + std::to_string(m_nodes[i].get_value()) + ")";
    }
    result += "\n";
    for (unsigned int i = NB_IN_NODES; i < hidden_node_beginning; i++) {
      result += "(" + std::to_string(m_nodes[i].get_value()) + ")";
    }
    result += "\nConnections:\n";
    for (unsigned int i = 0; i < m_nb_connections; i++) {
      result += "(" + std::to_string(m_connections[i].idx_node_in) + ") --> (" +
                std::to_string(m_connections[i].idx_node_out) + ")\n";
    }
    return result;
  }

  void mutate() {
    // small mutation
    if (rand_0_1() < 0.9) {
      change_connection_weight(0.05);
      mutate_random_node(0.05);
    }
    // sometimes, big ones
    if (rand_0_1() < 0.1) {
      change_connection_weight(0.5);
      mutate_random_node(0.5);
    }

    // topological mutations
    if (rand_0_1() < 0.01) {
      add_random_connection();
    }
    if (rand_0_1() < 0.01) {
      toggle_random_connection();
    }
    if (rand_0_1() < 0.005) {
      add_random_node();
    }
  }

private:
  void set_to_zero() {
    for (Node &node : m_nodes) {
      node.init();
    }
  }

  inline std::array<double, NB_OUT_NODES> extract_output() const {
    std::array<double, NB_OUT_NODES> output;
    for (unsigned int i = 0; i < NB_OUT_NODES; i++) {
      output[i] = m_nodes.at(i + NB_IN_NODES).value; // /!\ const correctness
    }
    return output;
  }

  inline void activate_last_layer() {
    for (unsigned int i = 0; i < NB_OUT_NODES; i++) {
      m_nodes[i + NB_IN_NODES].activate();
    }
  }

  inline void store_input(std::array<double, NB_IN_NODES> input) {
    for (unsigned int i = 0; i < NB_IN_NODES; i++) {
      m_nodes[i].set_value(input[i]);
    }
  }

  inline void activate_all_connections() {
    for (const Connection &current_connection : m_connections) {
      if (current_connection.enabled) {
        m_nodes[current_connection.idx_node_out].add_to_value(
            current_connection.weight *
            m_nodes[current_connection.idx_node_in].activate());
      }
    }
  }

  void add_random_node() {
    /* Selects a random connection and splits it in half by adding
     * a node in the middle.
     */
    unsigned int idx_new_node = NB_IN_NODES + NB_OUT_NODES + m_nb_hidden_nodes;
    m_nodes.emplace_back(0.); // small effect node
    m_nb_hidden_nodes++;

    unsigned int rnd_connection = rnd_int_smaller_than(m_nb_connections);
    Connection &split_connection = m_connections[rnd_connection];
    m_connections.emplace_back(idx_new_node, split_connection.idx_node_out,
                               1.); // small effect connection
    m_nb_connections++;
    split_connection.idx_node_out = idx_new_node; // modify the split one
    sort_connections();
  }

  void add_random_connection() {
    // add a random connection by selecting two nodes
    unsigned int rnd_node_in_idx =
        rnd_int_smaller_than(NB_IN_NODES + m_nb_hidden_nodes);
    if (rnd_node_in_idx >= NB_IN_NODES) {
      rnd_node_in_idx += NB_OUT_NODES;
    }
    unsigned int rnd_node_out_idx =
        NB_OUT_NODES + rnd_int_smaller_than(NB_OUT_NODES + m_nb_hidden_nodes);
    if (rnd_node_in_idx == rnd_node_out_idx) {
      return;
    }

    // check whether the connection creates duplicates
    bool is_new = true;
    for (const Connection &connection : m_connections) {
      if (connection.idx_node_in == rnd_node_in_idx and
          connection.idx_node_out == rnd_node_out_idx) {
        is_new = false;
      }
    }
    if (!is_new) {
      return;
    }

    m_connections.emplace_back(rnd_node_in_idx, rnd_node_out_idx, 1.);
    m_nb_connections++;

    // sort the brain
    if (sort_connections()) {
      return;
    }
    // otherwise, it means that the brain is not possible
    m_connections.pop_back();
    m_nb_connections--;
  }

  void toggle_random_connection() {
    unsigned int rnd_connection = rnd_int_smaller_than(m_nb_connections);
    m_connections[rnd_connection].toggle_enabled();
  }

  void change_connection_weight(double factor) {
    unsigned int rnd_connection = rnd_int_smaller_than(m_nb_connections);
    m_connections[rnd_connection].mutate(factor);
  }

  void mutate_random_node(double factor) {
    unsigned int rnd_node =
        rnd_int_smaller_than(m_nb_hidden_nodes + NB_IN_NODES + NB_OUT_NODES);
    m_nodes[rnd_node].mutate(factor);
  }

  // TODO: needs to check
  bool sort_connections() {
    std::vector<Connection> connections_to_treat;
    std::vector<Connection> storing_area;
    for (unsigned int i = 0; i < m_nb_connections; i++) {
      connections_to_treat.push_back(m_connections[i]);
      storing_area[i] = m_connections[i];
    }
    Connection current_connection;
    unsigned int current_idx = 0;
    unsigned int nb_tries = 0;
    while (!connections_to_treat.empty() and nb_tries < m_nb_connections) {
      current_connection = connections_to_treat[0];
      connections_to_treat.erase(connections_to_treat.begin());
      bool is_ordered = true;
      for (Connection other_connection : connections_to_treat) {
        if (other_connection.idx_node_out == current_connection.idx_node_in) {
          is_ordered = false;
        }
      }
      if (is_ordered) {
        storing_area[current_idx] = current_connection;
        current_idx++;
      } else {
        connections_to_treat.push_back(current_connection);
        nb_tries++;
      }
    }
    if (connections_to_treat.empty()) {
      for (unsigned int i = 0; i < m_nb_connections; i++) {
        m_connections[i] = storing_area[i];
      }
      return true;
    }
    return false;
  }
};
