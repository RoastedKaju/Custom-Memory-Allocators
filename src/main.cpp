#include <iostream>

#include "linear_allocator.h"

int main() {
  LinearAllocator la(10);

  std::cout << "Starting Pointer : " << la.start_pointer_ << std::endl;

  std::cout << "First Allocation : " << la.Allocate(3, alignof(int))
            << std::endl;
  std::cout << "Second Allocation : " << la.Allocate(3, alignof(int))
            << std::endl;
  std::cout << "Third Allocation : " << la.Allocate(3, alignof(int))
            << std::endl;

  return EXIT_SUCCESS;
}