#pragma once
#include <cmath>

namespace Math {

constexpr float PI = 3.14159265358979323846f;
constexpr float M_PI_F = 3.14159265358979323846f;
constexpr float TAU = 2.0f * PI;
constexpr float EPSILON = 1e-5f;

constexpr float to_radians(float degrees) {
    return degrees * (PI / 180.0f);
}

constexpr float to_degrees(float radians) {
    return radians * (180.0f / PI);
}

inline bool is_equal_approx(float a, float b, float epsilon = EPSILON) {
    return std::fabs(a - b) <= epsilon;
}

inline bool is_zero_approx(float a, float epsilon = EPSILON) {
    return std::fabs(a) <= epsilon;
}

inline float wrap_angle(float radians) {
    return std::remainder(radians, TAU);
}

inline float angle_difference(float from, float to) {
    return wrap_angle(to - from);
}

template<typename T>
constexpr T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

constexpr float clamp01(float value) {
    return clamp(value, 0.0f, 1.0f);
}

constexpr float lerp(float from, float to, float t) {
    return from + (to - from) * t;
}

constexpr float lerp_clamped(float from, float to, float t) {
    return lerp(from, to, clamp01(t));
}

constexpr float inverse_lerp(float from, float to, float value) {
    return is_zero_approx(to - from) ? 0.0f : (value - from) / (to - from);
}

constexpr float move_towards(float current, float target, float max_delta) {
    return std::fabs(target - current) <= max_delta
        ? target
        : current + (target > current ? max_delta : -max_delta);
}

}
