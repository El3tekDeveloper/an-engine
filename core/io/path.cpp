#pragma once
#include "path.h"
#include "core/debug/logger.h"
#include <cstring>

namespace IO {

fs::path assets_path = "/home/el3tek/Projects/C-C++/an-engine/assets";
fs::path engine_path = "/home/el3tek/Projects/C-C++/an-engine";
fs::path user_path   = "/usr";

bool set_engine_path(const std::string& path) {
    fs::path p(path);
    if (!fs::exists(p)) {
        LOG_ERROR("Engine path does not exist: '{}'", path);
        return false;
    }
    if (!fs::is_directory(p)) {
        LOG_ERROR("Engine path is not a directory: '{}'", path);
        return false;
    }
    engine_path = fs::canonical(p);
    return true;
}

bool set_assets_path(const std::string& path) {
    fs::path p(path);
    if (!fs::exists(p)) {
        LOG_ERROR("Assets path does not exist: '{}'", path);
        return false;
    }
    if (!fs::is_directory(p)) {
        LOG_ERROR("Assets path is not a directory: '{}'", path);
        return false;
    }
    assets_path = fs::canonical(p);
    return true;
}

bool set_user_path(const std::string& path) {
    fs::path p(path);
    if (!fs::exists(p)) {
        LOG_ERROR("User path does not exist: '{}'", path);
        return false;
    }
    if (!fs::is_directory(p)) {
        LOG_ERROR("User path is not a directory: '{}'", path);
        return false;
    }
    user_path = fs::canonical(p);
    return true;
}

std::string resolve_path(const std::string& path) {
    auto convert = [&](const char* prefix,
        const std::filesystem::path& base) -> std::string {
        if (!path.starts_with(prefix)) return {};
        std::string local = path.substr(strlen(prefix));
        return (base / local).string();
    };

    if (auto p = convert("assets://", assets_path); !p.empty())
        return p;

    if (auto p = convert("engine://", engine_path); !p.empty())
        return p;

    if (auto p = convert("user://", user_path); !p.empty())
        return p;

    return path;
}

} // IO
