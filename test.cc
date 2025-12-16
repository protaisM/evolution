#include "experiment.h"
#include "map.h"
#include "mouse.h"
#include "predator.h"
#include "simulation.hpp"

int main() {
  srand(time(NULL));

  Circle my_map(1, true);
  // Square my_map(1, true);
  // Torus my_map(1);

  // SimpleMouse<1> my_mouse(&my_map, 0.8);
  // PositionAngle predator({{0.1, 0.2}, 0});
  // my_mouse.print();
  // my_mouse.advance(0.1, {predator});
  // my_mouse.print();

  // Logger my_log("test_log");

  // Predator::RectangleShaped_Static predator1(&my_map, 2.7, 0.7, 0.9, false);
  // Predator::CircleShaped_Bounce bouncy1(&my_map, 0.1, 0.4);
  // Predator::CircleShaped_Bounce bouncy2(&my_map, 0.1, 0.2);
  // Predator::CircleShaped_Bounce bouncy3(&my_map, 0.1, 0.4);
  // Predator::CircleShaped_RunInCircle circle(&my_map, 0.1, 0.1);
  // Predator::CircleShaped_RunInCircle circle2(&my_map, 0.1, 0.3);
  // Predator::CircleShaped_Straight on_torus(&my_map, 0.1, 0.5);
  // Experiment<SimpleMouse<1>, 1000> my_experiment("Circle wo bound", &my_map,
  //                                                &my_log, .5, 500, 200);
  // my_experiment.add_predator(&bouncy2);
  // my_experiment.add_predator(&circle);
  // my_experiment.run_and_display();
  // my_log.write_to_file();

  Application<SimpleMouse<1>, 1000> simu("Test simu");
  simu.run();

  return 0;
}
