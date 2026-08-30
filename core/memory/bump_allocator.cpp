#include "bump_allocator.h"
#include "core/debug/logger.h"
#include <cstddef>
#include <cstdlib>

BumpAllocator::BumpAllocator(size_t size) {
    begin = (std::byte*)malloc(size);

    if (!begin) {
        LOG_ERROR("BumpAllocator failed to allocate {} bytes!", size);
        current = nullptr;
        end = nullptr;
        return;
    }

    current = begin;
    end = begin + size;
}

BumpAllocator::~BumpAllocator() {
    begin = nullptr;
    current = nullptr;
    end = nullptr;
}

void* BumpAllocator::alloc(size_t bytes) {
    if (current + bytes > end) {
        LOG_ERROR("BumpAllocator out of memory!\
                Requested: {} bytes, Remaining: {} bytes", bytes, remaining());
        return nullptr;
    }

    void* result = current;
    current += bytes;
    
    return result;
}

void BumpAllocator::reset() {
    current = begin;
}

size_t BumpAllocator::used() const {
    return (size_t)(current - begin);
}
size_t BumpAllocator::capacity() const {
    return (size_t)(end - begin);
}
size_t BumpAllocator::remaining() const {
    return (size_t)(end - current);
}

