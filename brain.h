#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <vector>

inline unsigned int rnd_int_smaller_than(unsigned int bound) {
  return rand() % bound;
}

inline double rand_0_1() { return (double)std::rand() / ((double)RAND_MAX); }

class Node {
private:
  double m_value;

public:
  void init() { m_value = 0; }
  double get_value() const { return m_value; }
  void set_value(double x) { m_value = x; }
  double add_to_value(double x) {
    m_value += x;
    return m_value;
  }
};

struct Connection {
  double weight;
  double shift;
  unsigned int idx_node_in;
  unsigned int idx_node_out;

  Connection(unsigned int node_in, unsigned int node_out) {
    idx_node_in = node_in;
    idx_node_out = node_out;
    weight = ((double)rand() / RAND_MAX) * 2 - 1;
    shift = ((double)rand() / RAND_MAX) * 2 - 1;
  }

  Connection() {}

  void mutate(double mutation_strength) {
    weight += mutation_strength * (((double)rand() / RAND_MAX) * 2 - 1);
    shift += mutation_strength * (((double)rand() / RAND_MAX) * 2 - 1);
  }
};

template <unsigned int NB_IN_NODES, unsigned int NB_OUT_NODES,
          unsigned int MAX_BRAIN_SIZE, unsigned int MAX_NB_CONNECTIONS>
class Brain {
private:
  std::array<Node, MAX_BRAIN_SIZE> m_nodes;
  std::array<Connection, MAX_NB_CONNECTIONS> m_connections;
  unsigned int m_nb_hidden_nodes;
  unsigned int m_nb_connections;

public:
  Brain(unsigned int nb_hidden_nodes = 1)
      : m_nb_hidden_nodes(nb_hidden_nodes),
        m_nb_connections(NB_IN_NODES + NB_OUT_NODES) {
    /* The layout is the following:
     * m_nodes = [ input_nodes, output_nodes, hidden_nodes ]
     */
    if (nb_hidden_nodes + NB_IN_NODES + NB_OUT_NODES > MAX_BRAIN_SIZE or
        NB_IN_NODES + NB_OUT_NODES > MAX_NB_CONNECTIONS) {
      throw std::runtime_error("Invalid brain size");
      exit(1);
    }
    for (unsigned int i = 0; i < NB_IN_NODES; i++) {
      Node input_node;
      m_nodes[i] = input_node;
    }
    unsigned int hidden_node_beginning = NB_IN_NODES + NB_OUT_NODES;
    for (unsigned int i = NB_IN_NODES; i < hidden_node_beginning; i++) {
      Node output_node;
      m_nodes[i] = output_node;
    }
    for (unsigned int i = hidden_node_beginning;
         i < hidden_node_beginning + m_nb_hidden_nodes; i++) {
      Node hidden_node;
      m_nodes[i] = hidden_node;
    }

    /* Create all the connections:
     * any input or output node should be linked to a hidden node.
     * This process is random.
     */
    unsigned int hidden_node_idx;
    for (unsigned int i = 0; i < NB_IN_NODES; i++) {
      hidden_node_idx = rnd_int_smaller_than(m_nb_hidden_nodes);
      Connection connection(i, hidden_node_beginning + hidden_node_idx);
      m_connections[i] = connection;
    }
    for (unsigned int i = NB_IN_NODES; i < hidden_node_beginning; i++) {
      hidden_node_idx = rnd_int_smaller_than(m_nb_hidden_nodes);
      Connection connection(hidden_node_beginning + hidden_node_idx, i);
      m_connections[i] = connection;
    }
  }

  std::array<double, NB_OUT_NODES>
  activate(std::array<double, NB_IN_NODES> input) {
    set_to_zero();
    print();
    for (unsigned int i = 0; i < NB_IN_NODES; i++) {
      m_nodes[i].set_value(input[i]);
    }
    for (unsigned int i = 0; i < m_nb_connections; i++) {
      Connection current_connection = m_connections[i];
      m_nodes[current_connection.idx_node_out].add_to_value(
          std::tanh(current_connection.weight *
                   m_nodes[current_connection.idx_node_in].get_value() +
               current_connection.shift));
    }
    std::array<double, NB_OUT_NODES> output;
    for (unsigned int i = 0; i < NB_OUT_NODES; i++) {
      output[i] = std::tanh(m_nodes[i + NB_IN_NODES].get_value());
    }
    return output;
  }

  void print() {
    std::cout << "Nodes:" << std::endl;
    for (unsigned int i = 0; i < NB_IN_NODES; i++) {
      std::cout << "(" << m_nodes[i].get_value() << ")";
    }
    std::cout << std::endl;
    unsigned int hidden_node_beginning = NB_IN_NODES + NB_OUT_NODES;
    for (unsigned int i = hidden_node_beginning;
         i < hidden_node_beginning + m_nb_hidden_nodes; i++) {
      std::cout << "(" << m_nodes[i].get_value() << ")";
    }
    std::cout << std::endl;
    for (unsigned int i = NB_IN_NODES; i < hidden_node_beginning; i++) {
      std::cout << "(" << m_nodes[i].get_value() << ")";
    }
    std::cout << std::endl << "Connections:" << std::endl;
    for (unsigned int i = 0; i < m_nb_connections; i++) {
      std::cout << "(" << m_connections[i].idx_node_in << ")" << "-->" << "("
                << m_connections[i].idx_node_out << ")" << std::endl;
    }
  }

  void mutate(double mutation_strength) {
    if (mutation_strength < 0 or mutation_strength > 1) {
      std::cerr << "The mutation strength should be between 0 and 1."
                << std::endl;
      return;
    }
    // mutations, by order of likelyhood
    double rnd = rand_0_1();
    if (rnd < mutation_strength) {
      // std::cout << "Random connection changed" << std::endl;
      change_connection_weight(mutation_strength);
    }
    rnd = rand_0_1();
    if (2 * rnd < mutation_strength) {
      // std::cout << "Random connection created" << std::endl;
      add_random_connection();
    }
    rnd = rand_0_1();
    if (3 * rnd < mutation_strength) {
      // std::cout << "Random node created" << std::endl;
      add_random_node();
    }
    rnd = rand_0_1();
    if (4 * rnd < mutation_strength) {
      // std::cout << "Random connection removed" << std::endl;
      remove_random_connection();
    }
  }

private:
  void set_to_zero() {
    for (Node &node : m_nodes) {
      node.init();
    }
  }

  void add_random_node() {
    /* Selects a random connection and splits it in half by adding
     * a node in the middle.
     */
    unsigned int idx_new_node = NB_IN_NODES + NB_OUT_NODES + m_nb_hidden_nodes;
    if (idx_new_node + 1 > MAX_BRAIN_SIZE) {
      return;
    }
    if (m_nb_connections + 1 > MAX_NB_CONNECTIONS) {
      return;
    }
    m_nb_hidden_nodes++;
    unsigned int rnd_connection = rnd_int_smaller_than(m_nb_connections);
    Connection new_connection(idx_new_node,
                              m_connections[rnd_connection].idx_node_out);
    m_connections[rnd_connection].idx_node_out = idx_new_node;
    m_connections[m_nb_connections] = new_connection;
    m_nb_connections++;
    sort_connections();
  }

  void add_random_connection() {
    // add a random connection by selecting two nodes
    if (m_nb_connections + 1 > MAX_NB_CONNECTIONS) {
      return;
    }
    unsigned int rnd_node_in_idx =
        rnd_int_smaller_than(NB_IN_NODES + m_nb_hidden_nodes);
    if (rnd_node_in_idx >= NB_IN_NODES) {
      rnd_node_in_idx += NB_OUT_NODES;
    }
    unsigned int rnd_node_out_idx =
        NB_OUT_NODES + rnd_int_smaller_than(NB_OUT_NODES + m_nb_hidden_nodes);
    // check whether the connection creates duplicates
    bool is_new_connection = true;
    for (unsigned int i = 0; i < m_nb_connections; i++) {
      if (m_connections[i].idx_node_in == rnd_node_in_idx and
          m_connections[i].idx_node_out == rnd_node_out_idx) {
        is_new_connection = false;
      }
    }
    if (is_new_connection and rnd_node_in_idx != rnd_node_out_idx) {
      Connection new_connection(rnd_node_in_idx, rnd_node_out_idx);
      m_connections[m_nb_connections] = new_connection;
      m_nb_connections++;
      // sort the brain
      if (sort_connections()) {
        return;
      }
      // otherwise, it means that the brain is not possible
      m_nb_connections--;
    }
  }

  void remove_random_connection() {
    if (m_nb_connections <= 1) {
      return;
    }
    unsigned int rnd_connection = rnd_int_smaller_than(m_nb_connections);
    for (unsigned int i = rnd_connection; i < m_nb_connections; i++) {
      m_connections[i] = m_connections[i + 1];
    }
    m_nb_connections--;
  }

  void change_connection_weight(double mutation_strength) {
    if (mutation_strength < 0 or mutation_strength > 1) {
      std::cerr << "The mutation strength should be between 0 and 1."
                << std::endl;
      return;
    }
    unsigned int rnd_connection = rnd_int_smaller_than(m_nb_connections);
    m_connections[rnd_connection].mutate(mutation_strength);
  }

  bool sort_connections() {
    std::vector<Connection> connections_to_treat;
    std::array<Connection, MAX_NB_CONNECTIONS> storing_area;
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
