#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <fstream>
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
  void store_begin(unsigned int generation_number) {
    m_generation_number.push_back(generation_number);
  }

  void store_end(double generation_duration, double survival_rate) {
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

  void plot(sf::RenderWindow *window, double window_width, double offset_width,
            double height_space) {
    double nb_plots = 6;
    double height_per_plot = height_space / nb_plots;
    double offset_height = 0;
    plot_quantity(window, m_survival_rate, "Survival rate", offset_width,
                  window_width - offset_width, offset_height, height_per_plot);
    offset_height = height_per_plot;
    plot_quantity(window, m_generation_duration, "Generation duration",
                  offset_width, window_width - offset_width, offset_height,
                  height_per_plot);
  }

private:
  void plot_quantity(sf::RenderWindow *window, std::vector<double> quantity,
                     std::string text_legend, double offset_width,
                     double plot_width, double offset_height,
                     double plot_height) {
    sf::Font font;
    font.loadFromFile("UbuntuMono-R.ttf");

    sf::Text legend;
    legend.setFont(font);
    legend.setString(" " + text_legend);
    legend.setFillColor(sf::Color::White);
    legend.setPosition(offset_width, offset_height);
    legend.setCharacterSize(20);

    // line around the plot
    sf::Color outline_color = sf::Color::Blue;
    sf::Vertex line_top[] = {
        sf::Vertex(sf::Vector2f(offset_width, offset_height), outline_color),
        sf::Vertex(sf::Vector2f(offset_width + plot_width, offset_height),
                   outline_color)};
    sf::Vertex line_right[] = {
        sf::Vertex(
            sf::Vector2f(offset_width + plot_width, offset_height + 0.0f),
            outline_color),
        sf::Vertex(sf::Vector2f(offset_width + plot_width,
                                offset_height + plot_height),
                   outline_color)};
    sf::Vertex line_bottom[] = {
        sf::Vertex(sf::Vector2f(offset_width + plot_width,
                                offset_height + plot_height),
                   outline_color),
        sf::Vertex(
            sf::Vector2f(offset_width + 0.0f, offset_height + plot_height),
            outline_color)};
    sf::Vertex line_left[] = {
        sf::Vertex(
            sf::Vector2f(offset_width + 0.0f, offset_height + plot_height),
            outline_color),
        sf::Vertex(sf::Vector2f(offset_width + 0.0f, offset_height + 0.0f),
                   outline_color)};

    unsigned int nb_data_point = m_generation_number.size() - 1;
    double zoom_width = plot_width / (nb_data_point - 1);
    zoom_width = std::min(zoom_width, 10.);
    double zoom_height = plot_height;
    sf::VertexArray line(sf::LinesStrip, nb_data_point);
    for (unsigned int x = 0; x < nb_data_point; ++x) {
      unsigned int y = (1 - quantity[x]) * zoom_height;
      line[x] = sf::Vertex(
          sf::Vector2f(offset_width + x * zoom_width, offset_height + y),
          sf::Color::White);
    }
    window->draw(legend);
    window->draw(line_right, 2, sf::Lines);
    window->draw(line_bottom, 2, sf::Lines);
    window->draw(line_top, 2, sf::Lines);
    window->draw(line_left, 2, sf::Lines);
    window->draw(line);
  }
};
