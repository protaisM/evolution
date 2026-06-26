#pragma once

#include <array>
#include <deque>
#include <iostream>
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
    m_connections.emplace_back(idx_new_node,
                               m_connections[rnd_connection].idx_node_out,
                               1.); // small effect connection
    m_nb_connections++;
    m_connections[rnd_connection].idx_node_out = idx_new_node;
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

  bool sort_connections() {
    /* topological sort */

    std::deque<Connection> to_treat;
    std::vector<Connection> sorted_arr;

    for (unsigned int i = 0; i < m_nb_connections; i++) {
      to_treat.push_back(m_connections[i]);
    }

    Connection current_connection;
    unsigned int nb_tries = 0; // trial counter

    while (!to_treat.empty()               // there is still connections to sort
           and nb_tries < m_nb_connections // to avoid cycles
    ) {

      current_connection = to_treat.front(); // take the first one to treat
      to_treat.pop_front();

      bool is_ordered = true;
      for (const Connection &other_connection : to_treat) {
        // check whether it can be inserted
        if (other_connection.idx_node_out == current_connection.idx_node_in) {
          is_ordered = false;
          break;
        }
      }
      if (is_ordered) {
        sorted_arr.push_back(current_connection); // insert
        nb_tries = 0;                             // reset the trial counter
      } else {
        to_treat.push_back(current_connection); // back in the queue
        nb_tries++;
      }
    }

    if (to_treat.empty()) {
      m_connections = sorted_arr;
      return true;
    }
    return false;
  }
};
