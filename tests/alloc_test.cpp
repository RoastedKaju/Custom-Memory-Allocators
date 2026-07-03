#include <gtest/gtest.h>

#include "stack_allocator.h"
#include "double_stack_allocator.h"
#include "pool_allocator.h"
#include "arena_allocator.h"

TEST(StackAllocator, AllocateSingleBlock) {
    StackAllocator allocator(128);

    void *ptr = allocator.allocate(16, alignof(std::max_align_t));

    EXPECT_NE(ptr, nullptr);
}

TEST(StackAllocator, AllocateMultipleBlocks) {
    StackAllocator allocator(128);

    void *a = allocator.allocate(16, 8);
    void *b = allocator.allocate(16, 8);
    void *c = allocator.allocate(16, 8);

    EXPECT_NE(a, nullptr);
    EXPECT_NE(b, nullptr);
    EXPECT_NE(c, nullptr);

    EXPECT_NE(a, b);
    EXPECT_NE(b, c);
    EXPECT_NE(a, c);
}

TEST(StackAllocator, ReturnedPointerIsProperlyAligned) {
    StackAllocator allocator(128);

    void *ptr = allocator.allocate(24, 32);

    ASSERT_NE(ptr, nullptr);

    const auto address = reinterpret_cast<uintptr_t>(ptr);

    EXPECT_EQ(address % 32, 0u);
}

TEST(StackAllocator, TemplateAllocateReturnsCorrectAlignment) {
    struct TestStruct {
        int a;
        float b;
    };

    StackAllocator allocator(128);

    auto *object = allocator.allocate<TestStruct>();

    ASSERT_NE(object, nullptr);

    const auto address = reinterpret_cast<uintptr_t>(object);

    EXPECT_EQ(address % alignof(TestStruct), 0u);
}

TEST(StackAllocator, AllocationFailsWhenOutOfMemory) {
    StackAllocator allocator(32);

    EXPECT_NE(allocator.allocate(16, 8), nullptr);
    EXPECT_NE(allocator.allocate(16, 8), nullptr);

    void *ptr = allocator.allocate(1, 8);

    EXPECT_EQ(ptr, nullptr);
}

TEST(StackAllocator, ResetAllowsReuse) {
    StackAllocator allocator(64);

    void *first = allocator.allocate(32, 8);

    ASSERT_NE(first, nullptr);

    allocator.reset();

    void *second = allocator.allocate(32, 8);

    EXPECT_EQ(first, second);
}

TEST(StackAllocator, OffsetIncreasesAfterAllocation) {
    StackAllocator allocator(128);

    EXPECT_EQ(allocator.offset(), 0u);

    allocator.allocate(8, 8);

    EXPECT_GT(allocator.offset(), 0u);
}

TEST(StackAllocator, OffsetResetsToZero) {
    StackAllocator allocator(128);

    allocator.allocate(16, 8);

    EXPECT_GT(allocator.offset(), 0u);

    allocator.reset();

    EXPECT_EQ(allocator.offset(), 0u);
}

TEST(StackAllocator, DifferentAlignmentsAreHandledCorrectly) {
    StackAllocator allocator(256);

    void *p1 = allocator.allocate(1, 1);
    void *p2 = allocator.allocate(1, 2);
    void *p3 = allocator.allocate(1, 4);
    void *p4 = allocator.allocate(1, 8);
    void *p5 = allocator.allocate(1, 16);
    void *p6 = allocator.allocate(1, 32);

    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    ASSERT_NE(p3, nullptr);
    ASSERT_NE(p4, nullptr);
    ASSERT_NE(p5, nullptr);
    ASSERT_NE(p6, nullptr);

    EXPECT_EQ(reinterpret_cast<uintptr_t>(p1) % 1, 0u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p2) % 2, 0u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p3) % 4, 0u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p4) % 8, 0u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p5) % 16, 0u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p6) % 32, 0u);
}

TEST(DoubleStackAllocator, AllocateFromBottom) {
    DoubleStackAllocator allocator(128);

    void *ptr = allocator.allocate_bottom(16, 8);

    ASSERT_NE(ptr, nullptr);
    EXPECT_GT(allocator.bottom_mark(), 0u);
}

TEST(DoubleStackAllocator, AllocateFromTop) {
    DoubleStackAllocator allocator(128);

    void *ptr = allocator.allocate_top(16, 8);

    ASSERT_NE(ptr, nullptr);
    EXPECT_LT(allocator.top_mark(), 128u);
}

TEST(DoubleStackAllocator, BottomAndTopAllocationsDoNotOverlap) {
    DoubleStackAllocator allocator(128);

    ASSERT_NE(allocator.allocate_bottom(32, 8), nullptr);
    ASSERT_NE(allocator.allocate_top(32, 8), nullptr);

    EXPECT_LT(allocator.bottom_mark(), allocator.top_mark());
}

TEST(DoubleStackAllocator, AllocationFailsWhenStacksMeet) {
    DoubleStackAllocator allocator(64);

    EXPECT_NE(allocator.allocate_bottom(32, 8), nullptr);
    EXPECT_NE(allocator.allocate_top(24, 8), nullptr);

    // No room should remain.
    EXPECT_EQ(allocator.allocate_bottom(16, 8), nullptr);
}

TEST(DoubleStackAllocator, TemplateAllocationReturnsAlignedPointers) {
    DoubleStackAllocator allocator(128);

    struct TestStruct {
        int x;
        float y;
    };

    auto *bottom = allocator.allocate_bottom<TestStruct>();
    auto *top = allocator.allocate_top<TestStruct>();

    ASSERT_NE(bottom, nullptr);
    ASSERT_NE(top, nullptr);

    EXPECT_EQ(reinterpret_cast<uintptr_t>(bottom) % alignof(TestStruct), 0u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(top) % alignof(TestStruct), 0u);
}

TEST(PoolAllocator, AllocateUntilFull) {
    struct TestObject {
        int value;
        float number;

        TestObject(const int v, const float n)
            : value(v), number(n) {
        }
    };

    PoolAllocator<TestObject> allocator(2);

    EXPECT_NE(allocator.allocate(), nullptr);
    EXPECT_NE(allocator.allocate(), nullptr);
    EXPECT_EQ(allocator.allocate(), nullptr);
}

TEST(PoolAllocator, ConstructInitializesObject) {
    struct TestObject {
        int value;
        float number;

        TestObject(int v, float n)
            : value(v), number(n) {
        }
    };

    PoolAllocator<TestObject> allocator(1);

    TestObject *obj = allocator.construct(42, 3.14f);

    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->value, 42);
    EXPECT_FLOAT_EQ(obj->number, 3.14f);

    allocator.destroy(obj);
}

TEST(PoolAllocator, DestroyReturnsSlotToPool) {
    struct TestObject {
        int value;
        float number;

        TestObject(int v, float n)
            : value(v), number(n) {
        }
    };

    PoolAllocator<TestObject> allocator(1);

    TestObject *first = allocator.construct(1, 2.0f);
    allocator.destroy(first);

    TestObject *second = allocator.construct(3, 4.0f);

    // The freed slot should be reused.
    EXPECT_EQ(first, second);

    allocator.destroy(second);
}

TEST(PoolAllocator, BufferIsCorrectlyAligned) {
    struct TestObject {
        int value;
        float number;

        TestObject(int v, float n)
            : value(v), number(n) {
        }
    };

    const PoolAllocator<TestObject> allocator(4);

    const auto address = reinterpret_cast<uintptr_t>(allocator.get_buffer());

    EXPECT_EQ(address % alignof(TestObject), 0u);
}
