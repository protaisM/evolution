#pragma once
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <vector>

unsigned int rnd_int_smaller_than(unsigned int bound) { return rand() % bound; }

class Node {
private:
  double m_value;

public:
  void init() { m_value = 0; }
  double get_value() const { return m_value; }
  void set_value(double x) { m_value = x; }
  double add_to_value(double x) {
    m_value += x;
    std::cout << "New value of the node: " << m_value << std::endl;
    return m_value;
  }
};

class Connection {
private:
  double m_weight;
  double m_shift;
  Node *m_node_in;
  Node *m_node_out;

public:
  Connection(Node *node_in, Node *node_out) {
    m_node_in = node_in;
    m_node_out = node_out;
    m_weight = ((double)rand() / RAND_MAX) * 2 - 1;
    m_shift = ((double)rand() / RAND_MAX) * 2 - 1;
    // std::cout << "Weight: " << m_weight << ", shift: " << m_shift <<
    // std::endl;
  }

  Connection() {}

  void activate() {
    double in = m_node_in->get_value();
    std::cout << "In: " << in;
    double out = tanh(m_weight * in + m_shift);
    std::cout << " --> " << out << std::endl;
    m_node_out->add_to_value(out);
  }

  void change_node_out(Node *new_node_out) { m_node_out = new_node_out; }
  Node *get_node_out() { return m_node_out; }
  Node *get_node_in() { return m_node_in; }
};

template <unsigned int NB_IN_NODES, unsigned int NB_OUT_NODES, unsigned int MAX_BRAIN_SIZE = 30,
          unsigned int MAX_NB_CONNECTIONS = 100>
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
      std::cout << "Hidden node idx: " << hidden_node_idx << std::endl;
      Node *start = &m_nodes[i];
      Node *end = &m_nodes[hidden_node_beginning + hidden_node_idx];
      std::cout << hidden_node_beginning + hidden_node_idx << " <-- " << i
                << std::endl;
      Connection connection(start, end);
      m_connections[i] = connection;
    }
    for (unsigned int i = NB_IN_NODES; i < hidden_node_beginning; i++) {
      hidden_node_idx = rnd_int_smaller_than(m_nb_hidden_nodes);
      std::cout << "Hidden node idx: " << hidden_node_idx << std::endl;
      Node *start = &m_nodes[hidden_node_beginning + hidden_node_idx];
      Node *end = &m_nodes[i];
      std::cout << hidden_node_beginning + hidden_node_idx << " --> " << i
                << std::endl;
      Connection connection(start, end);
      m_connections[i] = connection;
    }
  }

  std::array<double, NB_OUT_NODES>
  activate(std::array<double, NB_IN_NODES> input) {
    set_to_zero();
    for (unsigned int i = 0; i < NB_IN_NODES; i++) {
      m_nodes[i].set_value(input[i]);
    }
    for (unsigned int i = 0; i < m_nb_connections; i++) {
      m_connections[i].activate();
    }
    std::array<double, NB_OUT_NODES> output;
    for (unsigned int i = 0; i < NB_OUT_NODES; i++) {
      output[i] = m_nodes[i + NB_IN_NODES].get_value();
    }
    return output;
  }

  void print() {
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
    std::cout << std::endl;
  }

  void mutate() {
    unsigned int rnd = rnd_int_smaller_than(100);
    if (rnd % 4 == 0) {
      std::cout << "Random connection created" << std::endl;
      add_random_connection();
    }
    if (rnd % 4 == 1) {
      std::cout << "Random node created" << std::endl;
      add_random_node();
    }
    if (rnd % 4 == 2) {
      std::cout << "Random connection changed" << std::endl;
      change_connection_weight();
    }
    if (rnd % 15 == 0) {
      std::cout << "Random connection removed" << std::endl;
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
    // Node new_node;
    // m_nodes[NB_IN_NODES + NB_OUT_NODES + m_nb_hidden_nodes] = new_node;
    if (NB_IN_NODES + NB_OUT_NODES + m_nb_hidden_nodes + 1 > MAX_BRAIN_SIZE) {
      return;
    }
    if (m_nb_connections + 1 > MAX_NB_CONNECTIONS) {
      return;
    }
    Node *addr_new_node =
        &(m_nodes[NB_IN_NODES + NB_OUT_NODES + m_nb_hidden_nodes]);
    m_nb_hidden_nodes++;
    unsigned int rnd_connection = rnd_int_smaller_than(m_nb_connections);
    Connection *modified_connection = &(m_connections[rnd_connection]);
    Node *old_node_out = modified_connection->get_node_out();
    modified_connection->change_node_out(addr_new_node);
    Connection new_connection(addr_new_node, old_node_out);
    m_connections[m_nb_connections] = new_connection;
    m_nb_connections++;
    sort_connections();
  }

  void add_random_connection() {
    if (m_nb_connections + 1 > MAX_NB_CONNECTIONS) {
      return;
    }
    unsigned int rnd_node_in_idx =
        rnd_int_smaller_than(NB_IN_NODES + m_nb_hidden_nodes);
    unsigned int rnd_node_out_idx =
        rnd_int_smaller_than(NB_OUT_NODES + m_nb_hidden_nodes);
    Node *rnd_node_in = &(m_nodes[rnd_node_in_idx]);
    Node *rnd_node_out = &(m_nodes[NB_IN_NODES + rnd_node_out_idx]);
    Connection new_connection(rnd_node_in, rnd_node_out);
    m_connections[m_nb_connections] = new_connection;
    m_nb_connections++;
    if (sort_connections()) {
      return;
    }
    m_nb_connections--;
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

  void change_connection_weight() {
    unsigned int rnd_connection = rnd_int_smaller_than(m_nb_connections);
    Connection modified_connection = m_connections[rnd_connection];
    Connection new_connection(modified_connection.get_node_in(),
                              modified_connection.get_node_out());
    m_connections[rnd_connection] = new_connection;
  }

  bool sort_connections() {
    std::vector<Connection> connections_to_treat;
    for (unsigned int i = 0; i < m_nb_connections; i++) {
      connections_to_treat.push_back(m_connections[i]);
    }
    Connection current_connection;
    unsigned int current_idx = 0;
    unsigned int nb_tries = 0;
    while (!connections_to_treat.empty() and nb_tries < m_nb_connections) {
      current_connection = connections_to_treat[0];
      connections_to_treat.erase(connections_to_treat.begin());
      bool is_ordered = true;
      for (Connection other_connection : connections_to_treat) {
        if (other_connection.get_node_out() ==
            current_connection.get_node_in()) {
          is_ordered = false;
        }
      }
      if (is_ordered) {
        m_connections[current_idx] = current_connection;
        current_idx++;
      } else {
        connections_to_treat.push_back(current_connection);
        nb_tries++;
      }
    }
    if (connections_to_treat.empty()) {
      return true;
    }
    return false;
  }
};
