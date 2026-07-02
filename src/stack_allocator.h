#pragma once

#include <iostream>
#include <cstddef>
#include <cassert>

/**
 * Every variable and data has an alignment requirement, an 8-bit integer variable can be aligned to any address.
 * but a 32-bit or floating point must be 4 byte aligned.
 * Addresses can only end in nibbles `0x0, 0x4, 0x8 or 0xC` All memory allocators must be capable of returning
 * aligned memory blocks. To fix this simply allocate a bit more memory than required. In most implementations
 * the number of additional bytes allocated is equal to alignment minus one, which is the worst case alignment
 * shift we can have.
 *
 */

class StackAllocator {
public:
    explicit StackAllocator(const size_t size) : buffer_{nullptr},
                                                 size_{size},
                                                 offset_{0} {
        buffer_ = static_cast<std::byte *>(::operator new(size));
    }

    ~StackAllocator() {
        ::operator delete(buffer_);
    }

    StackAllocator(const StackAllocator &) = delete;

    StackAllocator &operator=(const StackAllocator &) = delete;

    StackAllocator(StackAllocator &&) = delete;

    StackAllocator &operator=(StackAllocator &&) = delete;

    void *allocate(const size_t size, const size_t alignment) {
        assert(std::has_single_bit(alignment) && "Alignment must be power of 2");
        // get current address by adding offset to starting address of buffer
        const auto current = reinterpret_cast<uintptr_t>(buffer_ + offset_);
        // now find the aligned address, using the formula
        const auto aligned = (current + alignment - 1) & ~(alignment - 1);
        // calculate how many bytes are padding
        const size_t padding = aligned - current;

        // check if we can add fit this into our stack
        if (offset_ + padding + size > size_) {
            return nullptr;
        }

        // update the new offset
        offset_ += padding;
        void *result = buffer_ + offset_;
        offset_ += size;

        return result;
    }

    // templated version
    template<typename T>
    T *allocate() {
        return static_cast<T *>(allocate(sizeof(T), alignof(T)));
    }

    void reset() {
        offset_ = 0;
    }

    [[nodiscard]] size_t offset() const { return offset_; }

private:
    std::byte *buffer_;
    size_t size_;
    size_t offset_;
};