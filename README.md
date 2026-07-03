# Allocators

---
This project contains header-only implementations of different memory allocators commonly used in real time
applications, the library provides templated functions for generic type safe allocation and freeing of memory.

## Types

- Stack
- Double-ended stack
- Pool
- Arena

## Explanation

**Stack Allocator:** Stack allocator pre-allocates a large chunk of memory using the C++ `::operator new` function,
the "stack pointer" in this case is just an offset value that tracks how much memory is occupied currently. Each type is
aligned to its own size of `alignof(T)`.  

The current address is noted: `current = (buffer + offset)`  
Find out the aligned address: `current + alignment - 1 & ~(alignment - 1)`  
Now you can easily find the padding: `padding = aligned - current`  

**Double Ended Stack Allocator:** In this stack we have two trackers one bottom and other top, it is mostly useful in
games where data is streamed in, we can keep static data at bottom while create and fill the top treating it more
dynamically.  
The alignment for bottom remains the same but there are some changes to the top:

```
const auto current = reinterpret_cast<uintptr_t>(buffer_) + bottom_offset_;
const auto aligned = (current + alignment - 1) & ~(alignment - 1);
```

**Pool Allocator:** The quality of pool allocator is all the elements inside it should be of same type,
this makes it very easy for us to reuse the free slots without the risk of causing fragmentation of our memory.
However, we will need to maintain a free list.

**Arena Allocator:** Arena allocator is almost the same as stack in this case, just one chunk of memory you populate and
free at once.

## Built With

- **Languages:** C++ 20
- **Build System:** CMake
- **Target Platform:** Windows
- **Third Party:** Google Test

## Getting Started

- Clone this repository recursively `git clone --recursive <repo-url>`
- Open the directory using CLion or Visual Studio Code
- Select Generator and Toolchain
- Configure
- Build and Run
