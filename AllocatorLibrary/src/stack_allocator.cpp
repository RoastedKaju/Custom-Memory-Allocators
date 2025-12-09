#include "stack_allocator.h"

#include <cassert>
#include <cstdlib>
#include <new>

StackAllocator::StackAllocator(size_t total_size)
    : total_size_(total_size), offset_(0), starting_ptr_(nullptr) {

  starting_ptr_ = std::malloc(total_size_);
  if (!starting_ptr_) {
    throw std::bad_alloc();
  }
}

StackAllocator::~StackAllocator() {
  std::free(starting_ptr_);
  starting_ptr_ = nullptr;
}

void* StackAllocator::Allocate(size_t size, size_t align) {
  // assert that alignment is power of 2
  assert((align & (align - 1)) == 0);

  // starting base
  uintptr_t base = reinterpret_cast<uintptr_t>(starting_ptr_) + offset_;

  // Reserve space for our header
  size_t header_size = sizeof(AllocationHeader);
  uintptr_t new_base = base + header_size;

  // Align the new base
  uintptr_t aligned = (new_base + (align - 1)) & ~(align - 1);
  size_t padding = aligned - base - header_size;

  // total size required
  size_t total_required = header_size + padding + size;

  if (offset_ + total_required > total_size_) {
    return nullptr;
  }

  // Write the metadata to header
  AllocationHeader* header = reinterpret_cast<AllocationHeader*>(
      reinterpret_cast<uint8_t*>(starting_ptr_) + offset_);

  header->padding_ = padding;
  header->size_ = size;

  void* result =
      reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(starting_ptr_) +
                              offset_ + header_size + padding);

  // Move the offset
  offset_ += total_required;

  return result;
}

void StackAllocator::Deallocate(void* ptr) {

  if (!ptr)
    return;

  // move back to the header
  auto* header = reinterpret_cast<AllocationHeader*>(
      reinterpret_cast<uint8_t*>(ptr) - sizeof(AllocationHeader));

  size_t total_to_free =
      header->size_ + header->padding_ + sizeof(AllocationHeader);

  assert(total_to_free <= offset_ && "Corrupted stack allocator state");

  // Roll back
  offset_ -= total_to_free;
}

size_t StackAllocator::AllocatedSize(void* ptr) {
  if (!ptr)
    return 0;
  auto* header = reinterpret_cast<AllocationHeader*>(
      reinterpret_cast<uint8_t*>(ptr) - sizeof(AllocationHeader));
  return header->size_;
}

void StackAllocator::Reset() {
  offset_ = 0;
}
