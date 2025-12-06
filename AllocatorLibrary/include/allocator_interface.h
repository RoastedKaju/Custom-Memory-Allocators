#ifndef ALLOCATOR_INTERFACE_H
#define ALLOCATOR_INTERFACE_H

class IAllocator {
 public:
  virtual void* Allocate(size_t size, size_t align) = 0;
  virtual void Deallocate(void* ptr) = 0;
  virtual size_t AllocatedSize(void* ptr) = 0;
  virtual void Reset() = 0;
};

#endif  // !ALLOCATOR_INTERFACE_H
