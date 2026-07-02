#pragma once

#include <iostream>
#include <new>

class Arena {
public:
    explicit Arena(const size_t size) {
        capacity_ = size;
        offset_ = 0;
        buffer_ = static_cast<std::byte *>(::operator new(size));
    }

    ~Arena() {
        ::operator delete(buffer_);
    }

    void *allocate(const size_t size, const size_t alignment) {
        const uintptr_t current = reinterpret_cast<uintptr_t>(buffer_) + offset_;
        const uintptr_t aligned = align_forward(current, alignment);

        const size_t adjustment = aligned - current;

        if (offset_ + adjustment + size > capacity_) {
            throw std::bad_alloc();
        }

        offset_ += adjustment;

        void *result = buffer_ + offset_;

        offset_ += size;

        return result;
    }

    template<typename T>
    T *allocate() {
        return static_cast<T *>(allocate(sizeof(T), alignof(T)));
    }

    template<typename T, typename... Args>
    T *construct(Args &&... args) {
        T *memory = allocate<T>();

        return new(memory) T(std::forward<Args>(args)...);
    }

    void reset() {
        offset_ = 0;
    }

    [[nodiscard]] size_t get_offset() const { return offset_; }

private:
    static size_t align_forward(const uintptr_t ptr, const size_t alignment) {
        auto current_ptr = ptr;
        if (const auto mod = current_ptr % alignment; mod != 0) {
            current_ptr += alignment - mod;
        }

        return current_ptr;
    }

    std::byte *buffer_;
    size_t capacity_;
    size_t offset_;
};