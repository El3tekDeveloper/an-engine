#pragma once
#include <cmath>

struct [[nodiscard]] Vector2i {
    enum Axis {
        X = 0,
        Y = 1
    };

    int x{};
    int y{};
    
    // Constructors
    constexpr Vector2i() = default;

    constexpr Vector2i(int px, int py)
        : x(px), y(py) {}

    constexpr Vector2i(int all)
        : x(all), y(all) {}
 
    // Constants
    static const Vector2i Zero;
    static const Vector2i One;

    static const Vector2i Left;
    static const Vector2i Right;
    static const Vector2i Up;
    static const Vector2i Down;

    // Indexing
    int& operator[](int axis) {
        return (&x)[axis];
    }

    const int& operator[](int axis) const {
        return (&x)[axis];
    }

    // Arithmetic
    constexpr Vector2i operator+(const Vector2i& v) const {
        return {x + v.x, y + v.y};
    }

    constexpr Vector2i operator-(const Vector2i& v) const {
        return {x - v.x, y - v.y};
    }

    constexpr Vector2i operator*(int s) const {
        return {x * s, y * s};
    }

    constexpr Vector2i operator/(int s) const {
        return {x / s, y / s};
    }

    Vector2i& operator+=(const Vector2i& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2i& operator-=(const Vector2i& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2i& operator*=(int s) {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2i& operator/=(int s) {
        x /= s;
        y /= s;
        return *this;
    }

    constexpr Vector2i operator-() const {
        return {-x, -y};
    }

    // Compare
    constexpr bool operator==(const Vector2i& v) const {
        return x == v.x && y == v.y;
    }

    constexpr bool operator!=(const Vector2i& v) const {
        return !(*this == v);
    }

    // Math
    int length_squared() const {
        return x*x + y*y;
    }

    int length() const {
        return std::sqrt(length_squared());
    }

    void normalize() {
        int len = length();
        if(len == 0.0f) return;

        x /= len;
        y /= len;
    }

    Vector2i normalized() const {
        int len = length();

        if(len == 0.0f)
            return Zero;

        return {
            x / len,
            y / len
        };
    }

    int dot(const Vector2i& v) const {
        return x*v.x + y*v.y;
    }

    int cross(const Vector2i& v) const {
        return x*v.y - y*v.x;
    }

    int distance_to(const Vector2i& v) const {
        return (*this - v).length();
    }

    Vector2i direction_to(const Vector2i& v) const {
        return (v - *this).normalized();
    }

    Vector2i lerp(
        const Vector2i& to,
        int t
    ) const {
        return {
            x + (to.x - x) * t,
            y + (to.y - y) * t
        };
    }

    Axis min_axis() const {
        return x < y ? X : Y;
    }

    Axis max_axis() const {
        return x > y ? X : Y;
    }
};


inline const Vector2i Vector2i::Zero  = {0,0};
inline const Vector2i Vector2i::One   = {1,1};

inline const Vector2i Vector2i::Left  = {-1,0};
inline const Vector2i Vector2i::Right = {1,0};
inline const Vector2i Vector2i::Up    = {0,-1};
inline const Vector2i Vector2i::Down  = {0,1};


inline constexpr Vector2i operator*(int s, const Vector2i& v) {
    return v * s;
}
