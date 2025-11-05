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
  for (int i = 0; i < 10; i++) {
    brain.mutate(0.5);
  }
  brain.print();
  result = brain.activate({1, 0, -1});
  brain.print();
  // std::cout<< res.m_x << "," << res.m_y<< std::endl;
  std::cout << "Beginning of the mouse : " << std::endl;
  SimpleMouse my_mouse(10, 1);
  my_mouse.print();
  my_mouse.advance(0.1, {0.1, 0.4});
  my_mouse.print();
  SimpleMouse other_one = my_mouse;
  other_one.print();
  other_one.advance(0.1, {0.1, 0.4});
  other_one.print();

  return 0;
}
