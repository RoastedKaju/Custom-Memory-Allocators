enable_testing()

include(FetchContent)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/heads/main.zip)

# Prevent GoogleTest from installing anything
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(Test)

target_sources(Test PRIVATE
        "alloc_test.cpp"
        "${CMAKE_SOURCE_DIR}/src/stack_allocator.h"
        "${CMAKE_SOURCE_DIR}/src/double_stack_allocator.h"
        "${CMAKE_SOURCE_DIR}/src/pool_allocator.h")

target_link_libraries(Test PRIVATE gtest gtest_main)

target_include_directories(Test PRIVATE "${CMAKE_SOURCE_DIR}/src")

include(GoogleTest)
gtest_discover_tests(Test)