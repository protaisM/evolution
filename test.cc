#include "brain.h"
#include "experiment.h"
#include "map.h"
#include "mouse.h"

#include <array>
#include <iostream>

int main() {
  srand(time(NULL));
  // Brain<3, 2, 30, 100> brain(2);
  // brain.print();
  // std::array<double, 2> result = brain.activate({1, 0, -1});
  // // brain.print();
  // // result = brain.activate({1, 0, -1});
  // brain.print();
  // for (int i = 0; i < 10; i++) {
  //   brain.mutate(0.5);
  //   brain.print();
  // }

  Square my_map(3);
  // SimpleMouse my_mouse([&my_map]() { return my_map.rnd_position(); }, 1, 1);
  // SimpleMouse other_one = my_mouse;
  // other_one.mutate(0.5);
  // my_mouse.print();
  // my_mouse.advance(0.1, {0.1, 0.4},
  //                  [&my_map](Position pos) { return my_map.is_in(pos); });
  // my_mouse.print();
  // other_one.print();
  // other_one.advance(0.1, {0.1, 0.4},
  //                   [&my_map](Position pos) { return my_map.is_in(pos); });
  // other_one.print();

  Experiment<SimpleMouse, Predator, Square, 100> my_experiment("Test", my_map);
  my_experiment.run_on_background(0.1);

  return 0;
}
