#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H

#include "allocator_interface.h"

/// <summary>
/// This is a simplest form of allocator where it uses an offset or a pointer to track the total used memory
/// All the memory is placed in a linear order, and it is not possible to free up specific chunks of memory, we only free the entire thing at the end
/// Fragmentation in this allocator is very low since all the items are placed linearly with their accurate size plus alignment
/// The Complexity is O(1)
/// It is also called a Bump Allocator
/// </summary>
class LinearAllocator : public IAllocator {
 public:
  // Ctor
  LinearAllocator(size_t total_size);
  ~LinearAllocator();

  // IAllocator interface
  void* Allocate(size_t size, size_t alignment) override;
  void Deallocate(void* ptr) override;
  size_t AllocatedSize(void* ptr) override;
  void Reset() override;
  // End IAllocator interface

 public:
  // Points to the start of memory block
  void* start_pointer_;
  // we move the offset every time we allocate
  size_t offset_;
  // total buffer size
  size_t total_size_;
};

#endif  // !LINEAR_ALLOCATOR_H
