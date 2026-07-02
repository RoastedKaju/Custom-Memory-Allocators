#pragma once

#include <iostream>
#include <cstddef>
#include <cassert>

/**
 * A single memory block can actually contain two stack allocators, one that fills from bottom and another from top
 * In games like Hydro thunder this stack was used to have bottom end for loading and unloading levels while top of
 * stack was used for temporary objects that were allocated and freed every frame, this type of stack has two markers
 *
 */

class DoubleStackAllocator {
public:
    explicit DoubleStackAllocator(const size_t size) : buffer_(nullptr),
                                                       size_(size),
                                                       bottom_offset_(0),
                                                       top_offset_(size) {
        buffer_ = static_cast<std::byte *>(::operator new(size));
    }

    ~DoubleStackAllocator() {
        ::operator delete(buffer_);
    }

    DoubleStackAllocator(const DoubleStackAllocator &) = delete;

    DoubleStackAllocator &operator=(const DoubleStackAllocator &) = delete;

    DoubleStackAllocator(DoubleStackAllocator &&) = delete;

    DoubleStackAllocator &operator=(DoubleStackAllocator &&) = delete;

    void *allocate_bottom(const std::size_t size, const std::size_t alignment) {
        assert(std::has_single_bit(alignment) && "Alignment must be power of 2");

        const auto current = reinterpret_cast<uintptr_t>(buffer_) + bottom_offset_;
        const auto aligned = (current + alignment - 1) & ~(alignment - 1);
        const size_t padding = aligned - current;

        // we make sure that our new allocation is less than the top offset mark
        if (bottom_offset_ + padding + size > top_offset_) {
            return nullptr;
        }

        // update offset
        bottom_offset_ += padding;
        void *result = buffer_ + bottom_offset_;
        bottom_offset_ += size;

        return result;
    }

    void *allocate_top(const std::size_t size, const std::size_t alignment) {
        assert(std::has_single_bit(alignment) && "Alignment must be power of 2");

        // prevent underflow
        if (size > top_offset_) {
            return nullptr;
        }

        const auto unaligned_addr = reinterpret_cast<uintptr_t>(buffer_) + top_offset_ - size;
        const auto aligned_addr = unaligned_addr & ~(alignment - 1);
        const size_t new_top_offset = aligned_addr - reinterpret_cast<uintptr_t>(buffer_);

        // check top mark with bottom mark
        if (new_top_offset < bottom_offset_) {
            return nullptr;
        }

        // update top offset
        top_offset_ = new_top_offset;
        return buffer_ + top_offset_;
    }

    // template allocate bottom function
    template<typename T>
    T *allocate_bottom() {
        return static_cast<T *>(allocate_bottom(sizeof(T), alignof(T)));
    }

    template<typename T>
    T *allocate_top() {
        return static_cast<T *>(allocate_top(sizeof(T), alignof(T)));
    }

    [[nodiscard]] size_t bottom_mark() const { return bottom_offset_; }
    [[nodiscard]] size_t top_mark() const { return top_offset_; }

private:
    std::byte *buffer_;
    size_t size_;
    size_t bottom_offset_;
    size_t top_offset_;
};
