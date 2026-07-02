cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(Allocators LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        "src/main.cpp"
        "src/stack_allocator.h"
        "src/double_stack_allocator.h"
        "src/pool_allocator.h")