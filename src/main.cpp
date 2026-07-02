#include <iostream>
#include <algorithm>

#include "stack_allocator.h"
#include "double_stack_allocator.h"
#include "pool_allocator.h"
#include "arena_allocator.h"

struct GameObject {
    double experience_;
    float hit_points_;
    float damage_;
};

int main() {
    printf("'*' means empty space.\n");
    {
        printf("====[STACK ALLOCATOR EXAMPLE]====\n");
        StackAllocator stack_allocator(64);

        // use the templated version
        auto *new_location = stack_allocator.allocate<float>();
        // placement new
        [[maybe_unused]] const auto *value = new(new_location) float(15.0f);

        std::printf("[");
        for (auto i = 0; i < 64; i++) {
            if (i < stack_allocator.offset()) {
                std::printf("x");
            } else {
                std::printf("*");
            }
        }
        std::printf("]\n");
    }
    printf("====[DOUBLE ENDED ALLOCATOR EXAMPLE]====\n");
    {
        DoubleStackAllocator double_ended_stack{64};

        double_ended_stack.allocate_bottom<int>();
        double_ended_stack.allocate_top<double>();

        // show stack allocator
        std::printf("[");
        for (auto i = 0; i < double_ended_stack.bottom_mark(); ++i) {
            std::printf("x");
        }
        for (auto i = double_ended_stack.bottom_mark(); i < double_ended_stack.top_mark(); ++i) {
            std::printf("*");
        }
        for (auto i = double_ended_stack.top_mark(); i < 64; ++i) {
            std::printf("z");
        }
        std::printf("]\n");
    }
    printf("====[POOL ALLOCATOR EXAMPLE]====\n");
    {
        PoolAllocator<GameObject> pool{64};

        pool.construct(GameObject{});
        pool.construct(GameObject{});

        std::printf("[");
        for (size_t i = 0; i < 64; ++i) {
            void *slot = static_cast<std::byte *>(pool.get_buffer()) + i * pool.get_element_size();

            // check if slot is in free list
            if (std::ranges::find(pool.get_free_list_ref(), slot) != pool.get_free_list_ref().end()) {
                std::printf("*"); // free slot
            } else {
                std::printf("x"); // allocated slot
            }
        }
        std::printf("]\n");
    }
    printf("====[ARENA ALLOCATOR EXAMPLE]====\n");
    {
        Arena allocator{64};

        allocator.construct<GameObject>(100.0f, 15.0);

        std::printf("[");
        for (auto i = 0; i < 64; ++i) {
            if (i < allocator.get_offset()) {
                std::printf("x");
            } else {
                std::printf("*");
            }
        }
        std::printf("]\n");
    }
    return EXIT_SUCCESS;
}
