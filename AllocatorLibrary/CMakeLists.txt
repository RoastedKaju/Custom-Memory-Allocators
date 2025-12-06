message("Allocator Library Directory...")

add_library(AllocatorLibrary STATIC
                                    "src/linear_allocator.cpp"
                                    "include/linear_allocator.h"
                                    "include/allocator_interface.h")

target_include_directories(AllocatorLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)