#pragma once
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <vector>

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
  void activate() {
    double in = m_node_in->get_value();
    std::cout << "In: " << in;
    double out = tanh(m_weight * in + m_shift);
    // std::cout << " --> " << out;
    // out = tanh(out);
    std::cout << " --> " << out << std::endl;
    m_node_out->add_to_value(out);
  }

  Connection(Node *node_in, Node *node_out) {
    m_node_in = node_in;
    m_node_out = node_out;
    m_weight = ((double)rand() / RAND_MAX) * 2 - 1;
    m_shift = ((double)rand() / RAND_MAX) * 2 - 1;
    std::cout << "Weight: " << m_weight << ", shift: " << m_shift << std::endl;
  }

  Connection() {}
};

template <size_t NB_IN_NODES, size_t NB_OUT_NODES, size_t MAX_BRAIN_SIZE = 20,
          size_t MAX_NB_CONNECTIONS = 100>
class Brain {
private:
  std::array<Node, MAX_BRAIN_SIZE> m_nodes;
  std::array<Connection, MAX_NB_CONNECTIONS> m_connections;
  size_t m_nb_hidden_nodes;
  size_t m_nb_connections;

public:
  void init() {
    for (Node &node : m_nodes) {
      node.init();
    }
  }

  Brain(size_t nb_hidden_nodes = 1)
      : m_nb_hidden_nodes(nb_hidden_nodes),
        m_nb_connections(NB_IN_NODES + NB_OUT_NODES) {
    /* The layout is the following:
     * m_nodes = [ input_nodes, output_nodes, hidden_nodes ]
     */
    if (nb_hidden_nodes + NB_IN_NODES + NB_OUT_NODES > MAX_BRAIN_SIZE or
        NB_IN_NODES + NB_OUT_NODES > MAX_NB_CONNECTIONS) {
      throw std::runtime_error("Size too big for the brain.");
      exit(1);
    }
    for (size_t i = 0; i < NB_IN_NODES; i++) {
      Node input_node;
      m_nodes[i] = input_node;
    }
    size_t hidden_node_beginning = NB_IN_NODES + NB_OUT_NODES;
    for (size_t i = NB_IN_NODES; i < hidden_node_beginning; i++) {
      Node output_node;
      m_nodes[i] = output_node;
    }
    for (size_t i = hidden_node_beginning;
         i < hidden_node_beginning + m_nb_hidden_nodes; i++) {
      Node hidden_node;
      m_nodes[i] = hidden_node;
    }

    /* Create all the connections:
     * any input or output node should be linked to a hidden node.
     */
    size_t hidden_node_idx;
    for (size_t i = 0; i < NB_IN_NODES; i++) {
      hidden_node_idx = arc4random_uniform(m_nb_hidden_nodes);
      std::cout << "Hidden node idx: " << hidden_node_idx << std::endl;
      Node *start = &m_nodes[i];
      Node *end = &m_nodes[hidden_node_beginning + hidden_node_idx];
      std::cout << hidden_node_beginning + hidden_node_idx << " <-- " << i
                << std::endl;
      Connection connection(start, end);
      m_connections[i] = connection;
    }
    for (size_t i = NB_IN_NODES; i < hidden_node_beginning; i++) {
      hidden_node_idx = arc4random_uniform(m_nb_hidden_nodes);
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
    init();
    for (size_t i = 0; i < NB_IN_NODES; i++) {
      m_nodes[i].set_value(input[i]);
    }
    for (size_t i = 0; i < m_nb_connections; i++) {
      m_connections[i].activate();
    }
    std::cout << "OK" << std::endl;
    std::array<double, NB_OUT_NODES> output;
    for (size_t i = 0; i < NB_OUT_NODES; i++) {
      output[i] = m_nodes[i + NB_IN_NODES].get_value();
    }
    return output;
  }

  void print() {
    for (size_t i = 0; i < NB_IN_NODES; i++) {
      std::cout << "(" << m_nodes[i].get_value() <<")";
    }
    std::cout << std::endl;
    size_t hidden_node_beginning = NB_IN_NODES + NB_OUT_NODES;
    for (size_t i = hidden_node_beginning;
         i < hidden_node_beginning + m_nb_hidden_nodes; i++) {
      std::cout  << "(" << m_nodes[i].get_value() << ")" ;
    }
    std::cout << std::endl;
    for (size_t i = NB_IN_NODES; i < hidden_node_beginning; i++) {
      std::cout << "("  << m_nodes[i].get_value() << ")" ;
    }
    std::cout << std::endl;
  }

  void add_node() {}
  void add_connection() {}
  void mutate() {}
};
