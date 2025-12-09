#include <iostream>

#include "linear_allocator.h"
#include "stack_allocator.h"

int main() {
  LinearAllocator la(10);

  std::cout << "Starting Pointer : " << la.start_pointer_ << std::endl;

  std::cout << "First Allocation : " << la.Allocate(3, alignof(int))
            << std::endl;
  std::cout << "Second Allocation : " << la.Allocate(3, alignof(int))
            << std::endl;
  std::cout << "Third Allocation : " << la.Allocate(3, alignof(int))
            << std::endl;

  std::cout << "====================== STACK ALLOCATOR ======================"
            << std::endl;

  StackAllocator sa(1024);

  void* a = sa.Allocate(32, 8);
  void* b = sa.Allocate(64, 16);

  std::cout << sa.AllocatedSize(a) << "\n";  // 32
  std::cout << sa.AllocatedSize(b) << "\n";  // 64

  sa.Deallocate(b);  // OK
  sa.Deallocate(a);  // OK

  sa.Reset();  // empties entire allocator

  return EXIT_SUCCESS;
}