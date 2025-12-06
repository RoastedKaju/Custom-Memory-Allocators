# Enable testing
enable_testing()

# Fetch GoogleTest
include(FetchContent)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/heads/main.zip)

# Prevent GoogleTest from installing anything
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Executeable for tests
add_executable(AllocatorTests "${CMAKE_CURRENT_SOURCE_DIR}/test_linear_allocator.cpp")

# Link Google Test + Allocator Library
target_link_libraries(AllocatorTests PRIVATE
    AllocatorLibrary
    gtest
    gtest_main
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(AllocatorTests)