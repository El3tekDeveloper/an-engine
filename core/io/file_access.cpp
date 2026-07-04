#include "core/io/file_access.h"
#include "core/debug/logger.h"
#include <fstream>

bool IO::exists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

size_t IO::get_file_size(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file) {
        LOG_ERROR("Failed to open file: '{}'", path);
        return 0;
    }

    return (size_t)file.tellg();
}

std::string IO::read_file(const std::string& path, BumpAllocator& allocator) { 
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file) {
        LOG_ERROR("Failed to open file: '{}'", path);
        return {};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* data = static_cast<char*>(allocator.alloc((size_t)size + 1));

    if (!data) {
        LOG_ERROR("Invalid data pointer (nullptr).");
        return {};
    }

    if (!file.read(data, size)) {
        LOG_ERROR("Failed to read {} bytes from file.", size);
        return {};
    }

    data[size] = '\0';
    return std::string(data, (size_t)size);
}

std::byte* IO::read_bytes(const std::string& path, BumpAllocator& allocator) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file) {
        LOG_ERROR("Failed to open file: '{}'", path);
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::byte* data = static_cast<std::byte*>(allocator.alloc((size_t)size));

    if (!data) {
        LOG_ERROR("Invalid data pointer (nullptr).");
        return nullptr;
    }

    if (!file.read(reinterpret_cast<char*>(data), size)) {
        LOG_ERROR("Failed to read {} bytes from file.", size);
        return nullptr;
    }

    return data;
}

bool IO::write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);

    if (!file)
        return false;

    file.write(content.data(), (std::streamsize)content.size());

    return file.good();
}

bool IO::write_bytes(const std::string& path, const void* data, size_t size) {
    if (!data || size == 0)
        return false;

    std::ofstream file(path, std::ios::binary);

    if (!file)
        return false;

    file.write(static_cast<const char*>(data), (std::streamsize)size);

    return file.good();
}

