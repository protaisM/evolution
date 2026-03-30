#pragma once

#include <cstdlib>

struct Color {
  int r;
  int g;
  int b;

  Color() {
    r = std::rand() % 255;
    g = std::rand() % 255;
    b = std::rand() % 255;
  }
  void mutate(){
    unsigned int rnd = std::rand() % 6;
    switch (rnd) {
    case 0: {
      r += 1;
      break;
    }
    case 1: {
      r -= 1;
      break;
    }
    case 2: {
      g += 1;
      break;
    }
    case 3: {
      g -= 1;
      break;
    }
    case 4: {
      b += 1;
      break;
    }
    case 5: {
      b -= 1;
      break;
    }
    }
  }
};
