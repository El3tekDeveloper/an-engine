#pragma once

namespace Math {

constexpr float PI = 3.14159265358979323846f;
constexpr float M_PI_F = 3.14159265358979323846f;
constexpr float TAU = 2.0f * PI;

constexpr float to_radians(float degrees) {
    return degrees * (PI / 180.0f);
}

constexpr float to_degrees(float radians) {
    return radians * (180.0f / PI);
}

}
