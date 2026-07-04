#pragma once
#include <cstdint>
#include <random>
#include <string>
#include <functional>
#include <vector>

using UUID = uint32_t;
constexpr UUID UUID_INVALID = 0;

inline UUID make_uuid(const std::string& path) {
    return (UUID)std::hash<std::string>{}(path);
}

inline UUID make_uuid() {
    static std::random_device rd;
    static std::mt19937_64 rng(rd());
    static std::uniform_int_distribution<UUID> dist;

    UUID id;
    do {
        id = dist(rng);
    } while (id == UUID_INVALID);

    return id;
}
