#pragma once
#include <cmath>

struct [[nodiscard]] Vector3i {
    enum Axis {
        X = 0,
        Y = 1,
        Z = 2
    };

    int x{};
    int y{};
    int z{};
    
    // Constructors
    constexpr Vector3i() = default;

    constexpr Vector3i(int px, int py, int pz)
        : x(px), y(py), z(pz) {}

    constexpr Vector3i(int all)
        : x(all), y(all), z(all) {}

    // Constants
    static const Vector3i Zero;
    static const Vector3i One;

    static const Vector3i Left;
    static const Vector3i Right;
    static const Vector3i Up;
    static const Vector3i Down;
    
    static const Vector3i Forward;
    static const Vector3i Backward;

    // Indexing
    int& operator[](int axis) {
        return (&x)[axis];
    }

    const int& operator[](int axis) const {
        return (&x)[axis];
    }

    // Arithmetic
    constexpr Vector3i operator+(const Vector3i& v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    constexpr Vector3i operator-(const Vector3i& v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    constexpr Vector3i operator*(int s) const {
        return {x * s, y * s, z * s};
    }

    constexpr Vector3i operator/(int s) const {
        return {x / s, y / s, z / s};
    }

    Vector3i& operator+=(const Vector3i& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3i& operator-=(const Vector3i& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3i& operator*=(int s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector3i& operator/=(int s) {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    constexpr Vector3i operator-() const {
        return {-x, -y, -z};
    }

    // Compare
    constexpr bool operator==(const Vector3i& v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    constexpr bool operator!=(const Vector3i& v) const {
        return !(*this == v);
    }

    // Math
    int length_squared() const {
        return x*x + y*y + z*z;
    }

    int length() const {
        return std::sqrt(length_squared());
    }

    void normalize() {
        int len = length();
        if(len == 0.0f) return;

        x /= len;
        y /= len;
        z /= len;
    }

    Vector3i normalized() const {
        int len = length();

        if(len == 0.0f)
            return Zero;

        return {
            x / len,
            y / len,
            z / len
        };
    }

    int dot(const Vector3i& v) const {
        return x*v.x + y*v.y + z*v.z;
    }

    int cross(const Vector3i& v) const {
        return x*v.y - y*v.x - z*v.z;
    }

    int distance_to(const Vector3i& v) const {
        return (*this - v).length();
    }

    Vector3i direction_to(const Vector3i& v) const {
        return (v - *this).normalized();
    }

    Vector3i lerp(
        const Vector3i& to,
        int t
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


inline const Vector3i Vector3i::Zero  = {0, 0, 0};
inline const Vector3i Vector3i::One   = {1, 1, 1};

inline const Vector3i Vector3i::Left  = {-1, 0, 0};
inline const Vector3i Vector3i::Right = { 1, 0, 0};

inline const Vector3i Vector3i::Up    = {0, 1, 0};
inline const Vector3i Vector3i::Down  = {0,-1, 0};

inline const Vector3i Vector3i::Forward  = {0, 0, 1};
inline const Vector3i Vector3i::Backward = {0, 0,-1};


inline constexpr Vector3i operator*(int s, const Vector3i& v) {
    return v * s;
}


