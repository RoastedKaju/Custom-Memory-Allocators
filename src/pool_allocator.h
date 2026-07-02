#pragma once

#include <iostream>
#include <vector>

/**
 * Pool allocator contains fixed size memory blocks, each type should have its own pool allocator.
 * It is very easy to implement and maintains a free list to reuse the slots already freed.
 * Use placement new to construct objects in place, for destruction manually call destructor and add the slot to
 * free list.
 *
 */

template<typename T>
class PoolAllocator {
public:
    explicit PoolAllocator(const size_t element_count) : element_count_(element_count),
                                                         free_list_(element_count) {
        element_size_ = sizeof(T);
        alignment_ = alignof(T);

        buffer_ = ::operator new(element_size_ * element_count_, static_cast<std::align_val_t>(alignment_));

        for (auto i = 0; i < element_count_; ++i) {
            free_list_[i] = static_cast<std::byte *>(buffer_) + i * element_size_;
        }
    }

    ~PoolAllocator() {
        ::operator delete(buffer_, static_cast<std::align_val_t>(alignment_));
    }

    T *allocate() {
        if (free_list_.empty()) {
            return nullptr;
        }
        void *slot = free_list_.back();
        free_list_.pop_back();
        return static_cast<T *>(slot);
    }

    template<typename... Args>
    T *construct(Args &&... args) {
        T *ptr = allocate();
        if (!ptr) {
            throw std::bad_alloc();
        }
        return new(ptr) T(std::forward<Args>(args)...);
    }

    void destroy(T *ptr) {
        ptr->~T();
        free_list_.push_back(ptr);
    }

    [[nodiscard]] size_t total_size() const {
        const size_t raw = element_size_ * element_count_;
        const size_t aligned = (raw + alignment_ - 1) & ~(alignment_ - 1);
        return aligned;
    }

    [[nodiscard]] void *get_buffer() const { return buffer_; }
    [[nodiscard]] size_t get_count() const { return element_count_; }
    [[nodiscard]] size_t get_element_size() const { return element_size_; }
    std::vector<void *> &get_free_list_ref() { return free_list_; }

private:
    void *buffer_;
    size_t element_count_;
    size_t element_size_;
    size_t alignment_;

    // bookkeeping
    std::vector<void *> free_list_;
};
