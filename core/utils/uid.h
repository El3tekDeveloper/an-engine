#pragma once
#include "core/io/path.h"
#include <cstdint>
#include <random>
#include <string>
#include <functional>

using UID = uint32_t;
constexpr UID UID_INVALID = 0;

inline UID make_uid(const std::string& path) {
    return (UID)std::hash<std::string>{}(IO::resolve_path(path));
}

inline UID make_uid() {
    static std::random_device rd;
    static std::mt19937_64 rng(rd());
    static std::uniform_int_distribution<UID> dist;

    UID id;
    do {
        id = dist(rng);
    } while (id == UID_INVALID);

    return id;
}
