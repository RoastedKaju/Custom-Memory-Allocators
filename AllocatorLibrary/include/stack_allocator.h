#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "allocator_interface.h"

class StackAllocator : public IAllocator {
 public:
  StackAllocator(size_t total_size);
  ~StackAllocator();

  // IAllocator Interface
  void* Allocate(size_t size, size_t align) override;
  void Deallocate(void* ptr) override;
  size_t AllocatedSize(void* ptr) override;
  void Reset() override;
  // ~IAllocator Interface

 private:
  size_t offset_;
  size_t total_size_;
  void* starting_ptr_;

  struct AllocationHeader {
    size_t padding_;
    size_t size_;
  };
};

#endif  // !STACK_ALLOCATOR_H
