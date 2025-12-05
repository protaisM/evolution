#include "experiment.h"
#include "map.h"
#include "mouse.h"
#include "predator.h"

int main() {
  srand(time(NULL));

  Circle my_map(1, true);
  // SimpleMouse<1> my_mouse(&my_map, 0.8);
  // Position predator({0.1, 0.2});
  // my_mouse.print();
  // my_mouse.advance(0.1, {predator});
  // my_mouse.print();

  Predator::CircleShaped_Bounce predator1(&my_map, 0.1, 0.2);
  Predator::CircleShaped_Bounce predator2(&my_map, 0.1, 0.3);
  // Predator::CircleShaped_Bounce predator3(&my_map, 0.1, 0.4);
  std::array<Predator::BasePredator *, 2> predators(
      {&predator1, &predator2});
  Experiment<SimpleMouse<2>, 1000, 2> my_experiment(
      "Circle w bound - 10^mut", &my_map, predators, 0.5, 2, 0.05, 200);
  my_experiment.run_and_display();

  return 0;
}
