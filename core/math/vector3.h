#pragma once
#include "core/math/vector_convert.h"
#include <cmath>
#include <functional>

struct [[nodiscard]] alignas(16) Vector3 {
    enum Axis {
        X = 0,
        Y = 1,
        Z = 2
    };

    float x{};
    float y{};
    float z{};
    
    // Constructors
    constexpr Vector3() = default;

    constexpr Vector3(float px, float py, float pz)
        : x(px), y(py), z(pz) {}

    constexpr Vector3(float all)
        : x(all), y(all), z(all) {}

    template<typename V> requires vecconv::ConvertibleFrom<V, Vector3>
    constexpr Vector3(const V& v)
    : x(static_cast<float>(vecconv::comp_x(v))),
      y(static_cast<float>(vecconv::comp_y(v))),
      z(static_cast<float>(vecconv::comp_z(v, 0.0f))) {};

    // Constants
    static const Vector3 Zero;
    static const Vector3 One;

    static const Vector3 Left;
    static const Vector3 Right;
    static const Vector3 Up;
    static const Vector3 Down;
    
    static const Vector3 Forward;
    static const Vector3 Backward;

    // Indexing
    float& operator[](int axis) {
        return (&x)[axis];
    }

    const float& operator[](int axis) const {
        return (&x)[axis];
    }

    // Arithmetic
    constexpr Vector3 operator+(const Vector3& v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    constexpr Vector3 operator-(const Vector3& v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    constexpr Vector3 operator*(float s) const {
        return {x * s, y * s, z * s};
    }

    constexpr Vector3 operator/(float s) const {
        return {x / s, y / s, z / s};
    }

    Vector3& operator+=(const Vector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3& operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector3& operator/=(float s) {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    constexpr Vector3 operator-() const {
        return {-x, -y, -z};
    }

    // Compare
    constexpr bool operator==(const Vector3& v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    constexpr bool operator!=(const Vector3& v) const {
        return !(*this == v);
    }

    // Math
    float length_squared() const {
        return x*x + y*y + z*z;
    }

    float length() const {
        return std::sqrt(length_squared());
    }

    void normalize() {
        float len = length();
        if(len == 0.0f) return;

        x /= len;
        y /= len;
        z /= len;
    }

    Vector3 normalized() const {
        float len = length();

        if(len == 0.0f)
            return Zero;

        return {
            x / len,
            y / len,
            z / len
        };
    }

    float dot(const Vector3& v) const {
        return x*v.x + y*v.y + z*v.z;
    }

    Vector3 cross(const Vector3& v) const {
        return {
            y*v.z - z*v.y,
            z*v.x - x*v.z,
            x*v.y - y*v.x
        };
    }

    float distance_to(const Vector3& v) const {
        return (*this - v).length();
    }

    Vector3 direction_to(const Vector3& v) const {
        return (v - *this).normalized();
    }

    Vector3 lerp(
        const Vector3& to,
        float t
    ) const {
        return {
            x + (to.x - x) * t,
            y + (to.y - y) * t,
            z + (to.z - z) * t
        };
    }

    Axis min_axis() const {
        if (x < y) {
            return x < z ? X : Z;
        } else {
            return y < z ? Y : Z;
        }
    }

    Axis max_axis() const {
        if (x > y) {
            return x > z ? X : Z;
        } else {
            return y > z ? Y : Z;
        }
    }
};

namespace std {

template<>
struct hash<Vector3> {
    size_t operator()(const Vector3& v) const noexcept {
        return hash<float>{}(v.x) ^ (hash<float>{}(v.y) << 1) ^ (hash<float>{}(v.z) << 2);
    }
};

}

inline const Vector3 Vector3::Zero  = {0, 0, 0};
inline const Vector3 Vector3::One   = {1, 1, 1};

inline const Vector3 Vector3::Left  = {-1, 0, 0};
inline const Vector3 Vector3::Right = { 1, 0, 0};

inline const Vector3 Vector3::Up    = {0, 1, 0};
inline const Vector3 Vector3::Down  = {0,-1, 0};

inline const Vector3 Vector3::Forward  = {0, 0, 1};
inline const Vector3 Vector3::Backward = {0, 0,-1};


inline constexpr Vector3 operator*(float s, const Vector3& v) {
    return v * s;
}

