#include "brain.h"
#include "mouse.h"
#include <array>
#include <iostream>

int main() {
  srand(time(NULL));
  Brain<3, 2> brain(2);
  // brain.set_to_zero();
  brain.print();
  std::array<double, 2> result = brain.activate({1, 0, -1});
  // brain.print();
  // result = brain.activate({1, 0, -1});
  brain.print();
  for (int i = 0; i < 10000; i++) {
    brain.mutate();
  }
  brain.print();
  result = brain.activate({1, 0, -1});
  brain.print();
  // std::cout<< res.m_x << "," << res.m_y<< std::endl;
  SimpleMouse my_mouse(0.1, 0.5);
  my_mouse.advance(0.1, {0.1, 0.4});
  my_mouse.kill();

  return 0;
}
