#pragma once

#include <string>
#include <filesystem>

namespace IO {
    namespace fs = std::filesystem;

    static inline std::string join(const std::string& path, const std::string& file) {
        return (fs::path(path) / file).string();
    }

    static inline std::string filename(const std::string& path) {
        return fs::path(path).filename().string();
    }
    static inline std::string extension(const std::string& path) {
        return fs::path(path).extension().string();
    }
    static inline std::string directory(const std::string& path) {
        return fs::path(path).parent_path().string();
    }
} // namespace io

