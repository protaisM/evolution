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

  void plot() {
    double window_width = sf::VideoMode::getDesktopMode().width;
    double window_height = sf::VideoMode::getDesktopMode().height;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Plot");

    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
          window.close();
        }
      }

      window.clear(sf::Color::White);
      plot_quantity(&window, m_generation_duration, 0, "Generation duration");
      plot_quantity(&window, m_survival_rate, 1, "Survival rate");
      window.display();
    }
  }

private:
  void plot_quantity(sf::RenderWindow *window, std::vector<double> quantity,
                     unsigned int plot_number, std::string text_legend) {
    sf::Font font;
    font.loadFromFile("UbuntuMono-R.ttf");

    double window_width = sf::VideoMode::getDesktopMode().width;
    double window_height = sf::VideoMode::getDesktopMode().height;

    double plot_width = window_width / 2;
    double plot_height = window_height / 4;

    double offset_height = plot_number * plot_height;
    double offset_width = 0;
    if (plot_number > 3) {
      offset_width = plot_width;
      offset_height -= 3 * plot_height;
    }

    sf::Text legend;
    legend.setFont(font);
    legend.setString(text_legend);
    legend.setFillColor(sf::Color::Black);
    legend.setPosition(offset_width, offset_height);
    legend.setCharacterSize(20);

    // line around the plot
    sf::Vertex line_top[] = {
        sf::Vertex(sf::Vector2f(offset_width, offset_height), sf::Color::Red),
        sf::Vertex(sf::Vector2f(offset_width + plot_width, offset_height),
                   sf::Color::Red)};
    sf::Vertex line_right[] = {
        sf::Vertex(
            sf::Vector2f(offset_width + plot_width, offset_height + 0.0f),
            sf::Color::Red),
        sf::Vertex(sf::Vector2f(offset_width + plot_width,
                                offset_height + plot_height),
                   sf::Color::Red)};
    sf::Vertex line_bottom[] = {
        sf::Vertex(sf::Vector2f(offset_width + plot_width,
                                offset_height + plot_height),
                   sf::Color::Red),
        sf::Vertex(
            sf::Vector2f(offset_width + 0.0f, offset_height + plot_height),
            sf::Color::Red)};
    sf::Vertex line_left[] = {
        sf::Vertex(
            sf::Vector2f(offset_width + 0.0f, offset_height + plot_height),
            sf::Color::Red),
        sf::Vertex(sf::Vector2f(offset_width + 0.0f, offset_height + 0.0f),
                   sf::Color::Red)};

    unsigned int nb_data_point = m_generation_number.size() - 1;
    double zoom_width = plot_width / (nb_data_point - 1);
    double zoom_height = plot_height;
    sf::VertexArray line(sf::LinesStrip, nb_data_point);
    for (unsigned int x = 0; x < nb_data_point; ++x) {
      unsigned int y = (1 - quantity[x]) * zoom_height;
      line[x] = sf::Vertex(
          sf::Vector2f(offset_width + x * zoom_width, offset_height + y),
          sf::Color::Black);
    }
    window->draw(legend);
    window->draw(line_right, 2, sf::Lines);
    window->draw(line_bottom, 2, sf::Lines);
    window->draw(line_top, 2, sf::Lines);
    window->draw(line_left, 2, sf::Lines);
    window->draw(line);
  }
};
