#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <numeric>
#include <string>
#include <vector>

void normalise(std::vector<double> &q) {
  double max = *std::max_element(q.begin(), q.end());
  for (double &e : q) {
    e /= max;
  }
}

class Logger {
private:
  std::string m_file;
  sf::Font m_font;

  std::vector<unsigned int> m_generation_number;
  std::vector<double> m_avg_fitness;
  std::vector<double> m_max_fitness;
  std::vector<double> m_min_fitness;
  std::vector<double> m_avg_brain_connections;
  std::vector<double> m_avg_brain_nodes;

public:
  Logger(std::string title) {
    m_file = title;
    m_font.loadFromFile("UbuntuMono-R.ttf");
  }
  void store(unsigned int generation_number, std::vector<double> const &fitness,
             std::vector<double> const &brain_connections,
             std::vector<double> const &brain_nodes) {

    m_generation_number.push_back(generation_number);

    m_max_fitness.push_back(*std::max_element(fitness.begin(), fitness.end()));
    m_min_fitness.push_back(*std::min_element(fitness.begin(), fitness.end()));
    m_avg_fitness.push_back(
        std::accumulate(fitness.begin(), fitness.end(), 0.) /
        (double)fitness.size());
    m_avg_brain_connections.push_back(std::accumulate(brain_connections.begin(),
                                                      brain_connections.end(),
                                                      0.) /
                                      (double)brain_connections.size());
    m_avg_brain_nodes.push_back(
        std::accumulate(brain_nodes.begin(), brain_nodes.end(), 0.) /
        (double)brain_nodes.size());
  }

  void write_to_file() {
    nlohmann::json json_struct;
    json_struct["generation"] = m_generation_number;
    json_struct["max_fitness"] = m_max_fitness;
    json_struct["min_fitness"] = m_min_fitness;
    json_struct["avg_fitness"] = m_avg_fitness;
    json_struct["brain_connections"] = m_avg_brain_connections;
    json_struct["brain_nodes"] = m_avg_brain_nodes;
    std::ofstream os(m_file + ".json");
    os << std::setw(4) << json_struct << std::endl;
  }

  void plot(sf::RenderWindow *window, sf::Vector2f offset, sf::Vector2f size) {
    unsigned int nb_plots = 6;
    float height_per_plot = size.y / nb_plots;

    float offset_height = 0;
    plot_quantity(window, m_avg_fitness, "Average fitness",
                  {offset.x, offset.y + offset_height},
                  {size.x, height_per_plot});
    offset_height += height_per_plot;
    plot_quantity(window, m_max_fitness, "Max fitness",
                  {offset.x, offset.y + offset_height},
                  {size.x, height_per_plot});
    offset_height += height_per_plot;
    plot_quantity(window, m_min_fitness, "Min fitness",
                  {offset.x, offset.y + offset_height},
                  {size.x, height_per_plot});
    offset_height += height_per_plot;
    plot_quantity(window, m_avg_brain_nodes, "Average number of nodes",
                  {offset.x, offset.y + offset_height},
                  {size.x, height_per_plot});
    offset_height += height_per_plot;
    plot_quantity(
        window, m_avg_brain_connections, "Average number of connections",
        {offset.x, offset.y + offset_height}, {size.x, height_per_plot});
  }

private:
  void plot_quantity(sf::RenderWindow *window, std::vector<double> quantity,
                     std::string text_legend, sf::Vector2f offset,
                     sf::Vector2f size) {
    offset += sf::Vector2f{4.0f, 2.0f};
    size -= sf::Vector2f{8.0f, 4.0f};
    sf::RectangleShape background(size);
    background.setPosition(offset);
    background.setFillColor(sf::Color(100.0f, 100.0f, 100.0f));
    window->draw(background);

    sf::Text legend;
    legend.setFont(m_font);
    legend.setString(" " + text_legend);
    legend.setFillColor(sf::Color::White);
    legend.setPosition(offset);
    legend.setCharacterSize(20);
    window->draw(legend);

    // line around the plot
    sf::Color outline_color = sf::Color::Blue;
    sf::Vertex lines[] = {
        sf::Vertex(offset, outline_color),
        sf::Vertex(offset + sf::Vector2f({size.x, 0.0f}), outline_color),
        sf::Vertex(offset + size, outline_color),
        sf::Vertex(offset + sf::Vector2f({0.0f, size.y}), outline_color),
        sf::Vertex(offset, outline_color)};
    window->draw(lines, 5, sf::LinesStrip);

    if (quantity.size() == 0) {
      return;
    }

    unsigned int nb_data_point = quantity.size() - 1;
    normalise(quantity);

    double zoom_width = size.x / (nb_data_point - 1);
    zoom_width = std::min(zoom_width, 10.);
    double zoom_height = size.y;
    if (m_generation_number.size() >= 1) {
      sf::VertexArray graph(sf::LinesStrip, nb_data_point);
      for (unsigned int x = 0; x < nb_data_point; ++x) {
        unsigned int y = (1 - quantity[x]) * zoom_height;
        graph[x] =
            sf::Vertex(sf::Vector2f(offset.x + x * zoom_width, offset.y + y),
                       sf::Color::White);
      }
      window->draw(graph);
    }
  }
};
