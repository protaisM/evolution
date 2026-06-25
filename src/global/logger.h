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
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Logger {
private:
  std::string m_file;

  // beginning of the day
  std::vector<unsigned int> m_generation_number;

  // end of the day
  std::vector<double> m_generation_duration; // in [0,1]
  std::vector<double> m_survival_rate;       // in [0,1]

public:
  Logger(std::string title) { m_file = title; }
  void store(unsigned int generation_number, double generation_duration,
             double survival_rate) {
    m_generation_number.push_back(generation_number);
    if (generation_duration > 1) {
      generation_duration = 1;
    }
    if (survival_rate > 1) {
      survival_rate = 1;
    }
    m_generation_duration.push_back(generation_duration);
    m_survival_rate.push_back(survival_rate);
  }

  void write_to_file() {
    nlohmann::json json_struct;
    json_struct["generations"] = m_generation_number;
    json_struct["duration"] = m_generation_duration;
    json_struct["survival_rate"] = m_survival_rate;
    std::ofstream os(m_file + ".json");
    os << std::setw(4) << json_struct << std::endl;
  }

  void plot(sf::RenderWindow *window, sf::Vector2f offset, sf::Vector2f size) {
    unsigned int nb_plots = 6;
    float height_per_plot = size.y / nb_plots;
    float offset_height = 0;
    plot_quantity(window, m_survival_rate, "Survival rate",
                  {offset.x, offset.y + offset_height},
                  {size.x, height_per_plot});
    offset_height = height_per_plot;
    plot_quantity(window, m_generation_duration, "Generation duration",
                  {offset.x, offset.y + offset_height},
                  {size.x, height_per_plot});
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

    sf::Font font;
    font.loadFromFile("UbuntuMono-R.ttf");

    sf::Text legend;
    legend.setFont(font);
    legend.setString(" " + text_legend);
    legend.setFillColor(sf::Color::White);
    legend.setPosition(offset);
    legend.setCharacterSize(20);

    // line around the plot
    sf::Color outline_color = sf::Color::Blue;
    sf::Vertex lines[] = {
        sf::Vertex(offset, outline_color),
        sf::Vertex(offset + sf::Vector2f({size.x, 0.0f}), outline_color),
        sf::Vertex(offset + size, outline_color),
        sf::Vertex(offset + sf::Vector2f({0.0f, size.y}), outline_color),
        sf::Vertex(offset, outline_color)};
    unsigned int nb_data_point = m_generation_number.size() - 1;
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
    window->draw(legend);
    window->draw(lines, 5, sf::LinesStrip);
  }
};
