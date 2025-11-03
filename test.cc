#include "brain.h"
#include <array>
#include <iostream>

int main() {
  srand(time(NULL));
  Brain<3, 2> brain(2);
  brain.init();
  brain.print();
  std::array<double, 2> result = brain.activate({1, 0, -1});
  brain.print();
  // std::cout<< res.m_x << "," << res.m_y<< std::endl;

  return 0;
}
