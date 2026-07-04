#pragma once
#include "core/memory/bump_allocator.h"
#include <cstddef>
#include <string>

namespace IO {
    bool exists(const std::string& path);
    size_t get_file_size(const std::string& path);
    
    std::string read_file(const std::string& path, BumpAllocator& allocator);
    bool write_file(const std::string& path, const std::string& content);

    std::byte* read_bytes(const std::string& path, BumpAllocator& allocator);
    bool write_bytes(const std::string& path, const void* data, size_t size);
} // namespace IO
