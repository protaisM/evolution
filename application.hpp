#pragma once

#include "experiment.h"
#include "logger.h"
#include "map.h"
#include "predator.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

enum Screen_type { FULL, ONLY_MAP, ONLY_LEGEND, EMPTY };

template <typename Mouse, unsigned int MICE_NUMBER> class Application {
private:
  char m_title[40];
  sf::RenderWindow *m_window;
  Experiment<Mouse, MICE_NUMBER> *m_experiment;
  Logger *m_logger;

  // for the display
  float m_map_display_size;
  Screen_type m_display = FULL;

public:
  Application(char title[40]) {
    strcpy(m_title, title);
    m_map_display_size = 940;
    m_logger = new Logger(title);
    m_experiment = new Experiment<Mouse, MICE_NUMBER>(m_logger);

    m_window = new sf::RenderWindow(
        sf::VideoMode(sf::VideoMode::getDesktopMode().width,
                      sf::VideoMode::getDesktopMode().height),
        std::string(m_title));
  }

  ~Application() {
    delete m_logger;
    delete m_window;
    delete m_experiment;
  }

  void run() {
    while (m_window->isOpen()) {
      sf::Event evnt;
      while (m_window->pollEvent(evnt)) {
        handle_event(m_window, evnt);
      }
      m_experiment->do_one_step();
      switch (m_display) {
      case FULL:
        full_dispay();
        break;
      default:
        break;
      }
    }
  }

private:
  void full_dispay() {
    sf::Vector2f offset({5.0f, 5.0f});
    m_window->clear(sf::Color::Black);
    m_experiment->draw(m_window, offset, m_map_display_size);
    m_experiment->draw_legend(m_window, offset);
    m_logger->plot(m_window, offset + sf::Vector2f({m_map_display_size, 0.0f}),
                   {sf::VideoMode::getDesktopMode().width - m_map_display_size,
                    (float)sf::VideoMode::getDesktopMode().height});
    m_window->display();
  }

  void handle_event(sf::RenderWindow *window, sf::Event evnt) {
    switch (evnt.type) {
    case sf::Event::Closed: // close the window
      window->close();
      break;
    case (sf::Event::KeyReleased):
      if (evnt.key.code == sf::Keyboard::Num1) {
        m_display = FULL;
      }
      if (evnt.key.code == sf::Keyboard::Num2) {
        m_display = ONLY_MAP;
      }
      if (evnt.key.code == sf::Keyboard::Num3) {
        m_display = ONLY_LEGEND;
        window->clear();
      }
      if (evnt.key.code == sf::Keyboard::Num4) {
        m_display = EMPTY;
        window->clear();
      }
      if (evnt.key.code == sf::Keyboard::J) {
        m_experiment->add_to_dt(-0.005);
      }
      if (evnt.key.code == sf::Keyboard::K) {
        m_experiment->add_to_dt(+0.005);
      }
      if (evnt.key.code == sf::Keyboard::Up) {
        m_experiment->add_to_generation_duration(10);
      }
      if (evnt.key.code == sf::Keyboard::Down) {
        m_experiment->add_to_generation_duration(-10);
      }
      if (evnt.key.code == sf::Keyboard::Right) {
        m_experiment->add_to_minimal_mice_number(10);
      }
      if (evnt.key.code == sf::Keyboard::Left) {
        m_experiment->add_to_minimal_mice_number(-10);
      }
      break;
    default:
      break;
    }
  }
};
