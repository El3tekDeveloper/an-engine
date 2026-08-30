#pragma once
#include "core/memory/bump_allocator.h"
#include <cstddef>
#include <string>

namespace IO {
    inline BumpAllocator& get_scratch_allocator(size_t min_size) {
        static thread_local BumpAllocator allocator(min_size);
        if (allocator.capacity() < min_size) {
            allocator = BumpAllocator(min_size);
        }
        return allocator;
    }

    bool exists(const std::string& path);
    size_t get_file_size(const std::string& path);
    
    std::string read_file(const std::string& path, BumpAllocator& allocator);
    std::string read_file(const std::string& path);
    
    bool write_file(const std::string& path, const std::string& content);

    std::byte* read_bytes(const std::string& path, BumpAllocator& allocator);
    std::byte* read_bytes(const std::string& path);

    bool write_bytes(const std::string& path, const void* data, size_t size);
} // namespace IO

