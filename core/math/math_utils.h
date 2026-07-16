#pragma once
#include <cstdlib>

constexpr float PI = 3.14159265358979323846f;
constexpr float M_PI_F = 3.14159265358979323846f;
constexpr float TAU = 2.0f * PI;

constexpr float to_radians(float degrees) {
    return degrees * (PI / 180.0f);
}

constexpr float to_degrees(float radians) {
    return radians * (180.0f / PI);
}

inline float random_range(float min_v, float max_v) {
    return min_v + (max_v - min_v) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}
