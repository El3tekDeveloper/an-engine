#pragma once
#include <cstddef>
#include <stdlib.h>
#include <string.h>

class BumpAllocator {
public:
    BumpAllocator(size_t size);
    ~BumpAllocator();

    void* alloc(size_t bytes);
    void reset();

    size_t used() const;
    size_t capacity() const;
    size_t remaining() const;

private:
    std::byte* begin = nullptr;
    std::byte* current = nullptr;
    std::byte* end = nullptr;
};
