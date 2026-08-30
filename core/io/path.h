#pragma once
#include <string>
#include <filesystem>

namespace IO {
    namespace fs = std::filesystem;

    enum class PathType {
        Native,
        Assets,
        Engine,
        User,
    };

    extern fs::path assets_path;
    extern fs::path engine_path;
    extern fs::path user_path;
    
    bool set_engine_path(const std::string& path);
    bool set_assets_path(const std::string& path);
    bool set_user_path(const std::string& path);
    std::string resolve_path(const std::string& path);

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

