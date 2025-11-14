#include "experiment.h"
#include "map.h"
#include "mouse.h"
#include "predator.h"

int main() {
  srand(time(NULL));
  // Brain<1, 2, 10, 100> brain(2);
  // brain.print();
  // std::array<double, 2> result = brain.activate({1, 0, -1});
  // // brain.print();
  // // result = brain.activate({1, 0, -1});
  // brain.print();
  // for (int i = 0; i < 10; i++) {
  //   brain.mutate(0.5);
  //   brain.print();
  // }

  // SimpleMouse my_mouse([&my_map]() { return my_map.rnd_position(); });
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

  Square my_map(1);
  Experiment<SimpleMouse, Predator::Bounce, Square, 1000> my_experiment("numerous", &my_map);
  // my_experiment.resume("cat_mice1");
  my_experiment.run_and_display(0.01);

  return 0;
}
