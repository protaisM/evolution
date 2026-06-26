#pragma once

#include "position.h"

#include <SFML/Graphics/Font.hpp>

struct DisplayParameters {
  // this struct holds all parameters related to the
  // display of the experiment
  unsigned int selected_mouse;
  unsigned int zoom;
  Position center_position;
  bool follow_mouse;

  sf::Font font;

  DisplayParameters() { font.loadFromFile("UbuntuMono-R.ttf"); }
};
