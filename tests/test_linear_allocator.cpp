#include <gtest/gtest.h>
#include "linear_allocator.h"

TEST(AllocatorTypeLinear, AllocateBasic) {

  LinearAllocator linear_allocator{10};

  EXPECT_NE(linear_allocator.Allocate(3, alignof(int)), nullptr);
  EXPECT_NE(linear_allocator.Allocate(3, alignof(int)), nullptr);
  EXPECT_NE(linear_allocator.Allocate(3, alignof(int)), nullptr);
}