#include "brain.h"

int main() {
  srand(time(NULL));

  for (int i = 0; i < 1; i++) {
    std::cout << "First brain" << std::endl;
    Brain<1, 1> brain;
    for (double x = 0; x <= 2; x += 0.1) {
      auto y = brain.activate({x});
      std::cout << x << " -> " << y[0] << '\n';
    }
    brain.print();

    std::cout << "After mutation" << std::endl;
    for (int j = 0; j < 500; j++) {
      brain.mutate();
    }
    brain.activate({1});
    brain.print();
  }

  return 0;
}
