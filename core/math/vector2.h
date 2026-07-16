#pragma once
#include <cmath>

struct [[nodiscard]] alignas(8) Vector2 {
    enum Axis {
        X = 0,
        Y = 1
    };

    float x{};
    float y{};
    
    // Constructors
    constexpr Vector2() = default;

    constexpr Vector2(float px, float py)
        : x(px), y(py) {}

    constexpr Vector2(float all)
        : x(all), y(all) {}

    // Constants
    static const Vector2 Zero;
    static const Vector2 One;

    static const Vector2 Left;
    static const Vector2 Right;
    static const Vector2 Up;
    static const Vector2 Down;

    // Indexing
    float& operator[](int axis) {
        return (&x)[axis];
    }

    const float& operator[](int axis) const {
        return (&x)[axis];
    }

    // Arithmetic
    constexpr Vector2 operator+(const Vector2& v) const {
        return {x + v.x, y + v.y};
    }

    constexpr Vector2 operator-(const Vector2& v) const {
        return {x - v.x, y - v.y};
    }

    constexpr Vector2 operator*(float s) const {
        return {x * s, y * s};
    }

    constexpr Vector2 operator/(float s) const {
        return {x / s, y / s};
    }

    Vector2& operator+=(const Vector2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2& operator*=(float s) {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2& operator/=(float s) {
        x /= s;
        y /= s;
        return *this;
    }

    constexpr Vector2 operator-() const {
        return {-x, -y};
    }

    // Compare
    constexpr bool operator==(const Vector2& v) const {
        return x == v.x && y == v.y;
    }

    constexpr bool operator!=(const Vector2& v) const {
        return !(*this == v);
    }

    // Math
    float length_squared() const {
        return x*x + y*y;
    }

    float length() const {
        return std::sqrt(length_squared());
    }

    void normalize() {
        float len = length();
        if(len == 0.0f) return;

        x /= len;
        y /= len;
    }

    Vector2 normalized() const {
        float len = length();

        if(len == 0.0f)
            return Zero;

        return {
            x / len,
            y / len
        };
    }

    float dot(const Vector2& v) const {
        return x*v.x + y*v.y;
    }

    float cross(const Vector2& v) const {
        return x*v.y - y*v.x;
    }

    float distance_to(const Vector2& v) const {
        return (*this - v).length();
    }

    Vector2 direction_to(const Vector2& v) const {
        return (v - *this).normalized();
    }

    Vector2 lerp(
        const Vector2& to,
        float t
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


inline const Vector2 Vector2::Zero  = {0,0};
inline const Vector2 Vector2::One   = {1,1};

inline const Vector2 Vector2::Left  = {-1,0};
inline const Vector2 Vector2::Right = {1,0};
inline const Vector2 Vector2::Up    = {0,-1};
inline const Vector2 Vector2::Down  = {0,1};


inline constexpr Vector2 operator*(float s, const Vector2& v) {
    return v * s;
}
