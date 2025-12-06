#include "experiment.h"
#include "map.h"
#include "mouse.h"
#include "predator.h"

int main() {
  srand(time(NULL));

  Circle my_map(1, false);
  // SimpleMouse<1> my_mouse(&my_map, 0.8);
  // Position predator({0.1, 0.2});
  // my_mouse.print();
  // my_mouse.advance(0.1, {predator});
  // my_mouse.print();

  Logger my_log("test_log");

  Predator::RectangleShaped_Static predator1(&my_map, 1., 1., 0.95, false);
  // Predator::CircleShaped_Bounce bouncy1(&my_map, 0.1, 0.03);
  // Predator::CircleShaped_Bounce bouncy2(&my_map, 0.1, 0.05);
  // Predator::CircleShaped_Bounce bouncy3(&my_map, 0.1, 0.1);
  // Predator::CircleShaped_RunInCircle circle(&my_map, 0.1, 0.1);
  // Predator::CircleShaped_RunInCircle circle2(&my_map, 0.1, 0.3);
  // Predator::CircleShaped_Straight on_torus(&my_map, 0.1, 0.5);
  std::array<Predator::BasePredator *, 1> predators({&predator1});
  Experiment<SimpleMouse<1>, 1000, 1> my_experiment(
      "Circle wo bound", &my_map, predators, &my_log, 0.5, 2, 0.1, 10);
  my_experiment.run_and_display();
  my_log.write_to_file();

  return 0;
}
