#include "linear_allocator.h"

#include <cstdlib>
#include <new>

LinearAllocator::LinearAllocator(size_t total_size)
    : start_pointer_(nullptr), offset_(0), total_size_(total_size) {
  // allocate the total size
  start_pointer_ = std::malloc(total_size_);

  if (!start_pointer_) {
    throw std::bad_alloc();
  }
}

LinearAllocator::~LinearAllocator() {
  std::free(start_pointer_);
  start_pointer_ = nullptr;
}

void* LinearAllocator::Allocate(size_t size, size_t alignment) {
  uintptr_t base = reinterpret_cast<uintptr_t>(start_pointer_) + offset_;
  uintptr_t aligned = (base + (alignment - 1)) & ~(alignment - 1);

  size_t padding = aligned - base;
  size_t final_size = padding + size;

  if (offset_ + final_size > total_size_) {
    // Out of memory
    return nullptr;
  }

  // Advance the offset with the size + padding
  offset_ += final_size;

  return reinterpret_cast<void*>(aligned);
}

void LinearAllocator::Deallocate(void* ptr) {
  // We cannot deallocate individual blocks in linear allocator
}

size_t LinearAllocator::AllocatedSize(void* ptr) {
  // Linear allocator cannot return indiviual allocated size
  return 0;
}

void LinearAllocator::Reset() {
  offset_ = 0;
}
