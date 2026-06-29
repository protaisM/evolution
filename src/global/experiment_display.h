#pragma once

#include "display_parameter.h"
#include "experiment.h"
#include "food.h"
#include "predator.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>

template <typename Mouse, unsigned int MICE_NUMBER> class Experiment;

template <typename Mouse, unsigned int MICE_NUMBER> class ExperimentDisplay {

private:
  DisplayParameters m_display_parameters;

public:
  ExperimentDisplay() = default;
  ExperimentDisplay(DisplayParameters params) : m_display_parameters(params) {}

  void draw(Experiment<Mouse, MICE_NUMBER> const &experiment,
            sf::RenderWindow *window, sf::Vector2f offset,
            float map_size) const {
    sf::RectangleShape background(sf::Vector2f({map_size, map_size}));
    background.setPosition(offset);
    background.setFillColor(sf::Color({80, 80, 80}));
    window->draw(background);
    for (size_t idx_mouse = 0; idx_mouse < MICE_NUMBER; idx_mouse++) {
      if (experiment.m_mice[idx_mouse].is_alive()) {
        bool is_selected_mouse =
            (m_display_parameters.selected_mouse == idx_mouse);
        experiment.m_mice[idx_mouse].draw(window, offset,
                                          m_display_parameters.zoom * map_size,
                                          is_selected_mouse);
      }
    }
    for (std::unique_ptr<Predator> const &predator :
         experiment.m_level.m_predators) {
      predator->draw(window, offset, m_display_parameters.zoom * map_size);
    }
    for (Food const &food : experiment.m_level.m_food) {
      food.draw(window, offset, m_display_parameters.zoom * map_size);
    }
    experiment.m_map->draw(window, offset, map_size);
  }

  void draw_legend(Experiment<Mouse, MICE_NUMBER> const &experiment,
                   sf::RenderWindow *window, sf::Vector2f offset) const {
    sf::Text legend;
    legend.setFont(m_display_parameters.font);
    std::string text_legend =
        " Gen " + std::to_string(experiment.m_params.generation) + " Mice " +
        std::to_string(experiment.m_params.nb_alive_mice) + " (" +
        std::to_string(experiment.m_params.minimal_mice_number) + ") / " +
        std::to_string(MICE_NUMBER) + "\n";
    text_legend += " Time " + std::to_string(experiment.m_params.time) + " (+" +
                   std::to_string(experiment.m_params.dt) + ") / " +
                   std::to_string(experiment.m_params.generation_duration) +
                   "\n";
    legend.setString(text_legend);
    legend.setFillColor(sf::Color::White);
    legend.setPosition(offset);
    legend.setCharacterSize(20);
    window->draw(legend);
  }
};
