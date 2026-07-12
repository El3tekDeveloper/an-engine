#pragma once
#include <cmath>

struct [[nodiscard]] Vector4 {
    enum Axis {
        X = 0,
        Y = 1,
        Z = 2,
        W = 3
    };

    float x{};
    float y{};
    float z{};
    float w{};
    
    // Constructors
    constexpr Vector4() = default;

    constexpr Vector4(float px, float py, float pz, float pw)
        : x(px), y(py), z(pz), w(pw) {}

    constexpr Vector4(float all)
        : x(all), y(all), z(all), w(all) {}

    // Constants
    static const Vector4 Zero;
    static const Vector4 One;

    static const Vector4 Left;
    static const Vector4 Right;
    static const Vector4 Up;
    static const Vector4 Down;
    
    static const Vector4 Forward;
    static const Vector4 Backward;

    // Indexing
    float& operator[](int axis) {
        return (&x)[axis];
    }

    const float& operator[](int axis) const {
        return (&x)[axis];
    }

    // Arithmetic
    constexpr Vector4 operator+(const Vector4& v) const {
        return {x + v.x, y + v.y, z + v.z, + v.w};
    }

    constexpr Vector4 operator-(const Vector4& v) const {
        return {x - v.x, y - v.y, z - v.z, w - v.w};
    }

    constexpr Vector4 operator*(float s) const {
        return {x * s, y * s, z * s, w * s};
    }

    constexpr Vector4 operator/(float s) const {
        return {x / s, y / s, z / s, w / s};
    }

    Vector4& operator+=(const Vector4& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector4& operator-=(const Vector4& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector4& operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector4& operator/=(float s) {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    constexpr Vector4 operator-() const {
        return {-x, -y, -z, -w};
    }

    // Compare
    constexpr bool operator==(const Vector4& v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    constexpr bool operator!=(const Vector4& v) const {
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

    Vector4 normalized() const {
        float len = length();

        if(len == 0.0f)
            return Zero;

        return {
            x / len,
            y / len,
            z / len,
            w / len
        };
    }

    float dot(const Vector4& v) const {
        return x*v.x + y*v.y + z*v.z;
    }

    float cross(const Vector4& v) const {
        return x*v.y - y*v.x - z*v.z;
    }

    float distance_to(const Vector4& v) const {
        return (*this - v).length();
    }

    Vector4 direction_to(const Vector4& v) const {
        return (v - *this).normalized();
    }

    Vector4 lerp(
        const Vector4& to,
        float t
    ) const {
        return {
            x + (to.x - x) * t,
            y + (to.y - y) * t,
            z + (to.z - z) * t,
            w + (to.w - w) * t
        };
    }

    Axis min_axis() const {
        Axis axis = X;
        float min = x;

        if (y < min) {
            min = y;
            axis = Y;
        }

        if (z < min) {
            min = z;
            axis = Z;
        }

        if (w < min) {
            axis = W;
        }

        return axis;
    }

    Axis max_axis() const {
        Axis axis = X;
        float max = x;

        if (y > max) {
            max = y;
            axis = Y;
        }

        if (z > max) {
            max = z;
            axis = Z;
        }

        if (w > max) {
            axis = W;
        }

        return axis;
    }
};


inline const Vector4 Vector4::Zero  = {0, 0, 0, 0};
inline const Vector4 Vector4::One   = {1, 1, 1, 0};

inline const Vector4 Vector4::Left  = {-1, 0, 0, 0};
inline const Vector4 Vector4::Right = { 1, 0, 0, 0};

inline const Vector4 Vector4::Up    = {0, 1, 0, 0};
inline const Vector4 Vector4::Down  = {0,-1, 0, 0};

inline const Vector4 Vector4::Forward  = {0, 0, 1, 0};
inline const Vector4 Vector4::Backward = {0, 0,-1, 0};


inline constexpr Vector4 operator*(float s, const Vector4& v) {
    return v * s;
}


