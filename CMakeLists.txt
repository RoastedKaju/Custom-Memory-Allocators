cmake_minimum_required(VERSION 3.24)

# CXX standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(AllocatorProject VERSION 1.0)

# Executeable
add_executable(${PROJECT_NAME} "src/main.cpp")

# Add Allocator Libarary sub-directory
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/AllocatorLibrary")

# Add tests
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Link allocator suite library
target_link_libraries(${PROJECT_NAME} PRIVATE AllocatorLibrary)