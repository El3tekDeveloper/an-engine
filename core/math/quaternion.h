// using Claude AI to save time
#pragma once
#include <cmath>
#include <format>
#include "math_utils.h"
#include "vector3.h"
#include "matrix4.h"

struct [[nodiscard]] Quaternion {

    struct EulerAngles {
        
        struct Proxy {
            EulerAngles* e;
            int axis;

            Proxy& operator=(float deg) {
                switch (axis) {
                    case 0: e->_pitch = deg; break;
                    case 1: e->_yaw   = deg; break;
                    default: e->_roll = deg; break;
                }

                e->_apply();
                return *this;
            }

            operator float() const {
                switch (axis) {
                    case 0: return e->_pitch;
                    case 1: return e->_yaw;
                    default: return e->_roll;
                }
            }

            Proxy& operator+=(float deg) { return *this = float(*this) + deg; }
            Proxy& operator-=(float deg) { return *this = float(*this) - deg; }
            Proxy& operator*=(float deg) { return *this = float(*this) * deg; }
            Proxy& operator/=(float deg) { return *this = float(*this) / deg; }

            bool operator==(const Proxy& p) const {
                return float(*this) == float(p);
            }

            bool operator!=(const Proxy& p) const {
                return !(*this == p);
            }
        };

        explicit EulerAngles(Quaternion* q)
            : owner(q), x{ this, 0 }, y{ this, 1 }, z{ this, 2 } {}

    private:
        Quaternion* owner;
        float _pitch{};
        float _yaw{};
        float _roll{};

        void _apply() {
            *owner = Quaternion::from_euler(
                Math::to_radians(_pitch),
                Math::to_radians(_yaw),
                Math::to_radians(_roll)
            );
        }

        friend struct Proxy;

    public:
        Proxy x;
        Proxy y;
        Proxy z;
    };
    
    enum Axis {
        X = 0,
        Y = 1,
        Z = 2,
        W = 3
    };

    EulerAngles euler;

private:

public:
    float _x{}, _y{}, _z{}, _w{ 1 };
    Quaternion()
        : euler(this) {}

    Quaternion(float px, float py, float pz, float pw)
        : euler(this), _x(px), _y(py), _z(pz), _w(pw) {}

    Quaternion(const Quaternion& o)
        : euler(this), _x(o._x), _y(o._y), _z(o._z), _w(o._w) {}

    Quaternion& operator=(const Quaternion& o) {
        _x = o._x;
        _y = o._y;
        _z = o._z;
        _w = o._w;
        return *this;
    }

    static const Quaternion Identity;

    static Quaternion Euler(float pitch, float yaw, float roll) {
        return from_euler(Math::to_radians(pitch), Math::to_radians(yaw), Math::to_radians(roll));
    }

    static Quaternion from_axis_angle(Vector3 p, float angle) {
        float half = angle * 0.5f;
        float s = std::sin(half);
        return { p.x*s, p.y*s, p.z*s, std::cos(half) };
    }

    static Quaternion from_euler(float pitch, float yaw, float roll) {
        float cp = std::cos(pitch * 0.5f), sp = std::sin(pitch * 0.5f);
        float cy = std::cos(yaw   * 0.5f), sy = std::sin(yaw   * 0.5f);
        float cr = std::cos(roll  * 0.5f), sr = std::sin(roll  * 0.5f);
        return {
            sr*cp*cy - cr*sp*sy,
            cr*sp*cy + sr*cp*sy,
            cr*cp*sy - sr*sp*cy,
            cr*cp*cy + sr*sp*sy
        };
    }

    static Quaternion from_matrix(const Matrix4& mat);

    float x() const { return _x; }
    float y() const { return _y; }
    float z() const { return _z; }
    float w() const { return _w; }

    Quaternion operator+(const Quaternion& q) const {
        return { _x+q._x, _y+q._y, _z+q._z, _w+q._w };
    }

    Quaternion operator-(const Quaternion& q) const {
        return { _x-q._x, _y-q._y, _z-q._z, _w-q._w };
    }

    Quaternion operator*(const Quaternion& q) const {
        return {
            _w*q._x + _x*q._w + _y*q._z - _z*q._y,
            _w*q._y - _x*q._z + _y*q._w + _z*q._x,
            _w*q._z + _x*q._y - _y*q._x + _z*q._w,
            _w*q._w - _x*q._x - _y*q._y - _z*q._z
        };
    }

    Quaternion operator*(float s) const {
        return { _x*s, _y*s, _z*s, _w*s };
    }

    Quaternion operator-() const {
        return { -_x, -_y, -_z, -_w };
    }

    Quaternion& operator+=(const Quaternion& q) { return *this = *this + q; }
    Quaternion& operator-=(const Quaternion& q) { return *this = *this - q; }
    Quaternion& operator*=(const Quaternion& q) { return *this = *this * q; }
    Quaternion& operator*=(float s)             { return *this = *this * s; }

    bool operator==(const Quaternion& q) const {
        return _x == q._x && _y == q._y && _z == q._z && _w == q._w;
    }

    bool operator!=(const Quaternion& q) const {
        return !(*this == q);
    }

    float length_squared() const {
        return _x*_x + _y*_y + _z*_z + _w*_w;
    }

    float length() const {
        return std::sqrt(length_squared());
    }

    Quaternion normalized() const {
        float len = length();
        if (len == 0.0f) return Identity;
        return { _x/len, _y/len, _z/len, _w/len };
    }

    void normalize() {
        *this = normalized();
    }

    Quaternion conjugate() const {
        return { -_x, -_y, -_z, _w };
    }

    Quaternion inversed() const {
        float ls = length_squared();
        if (ls == 0.0f) return Identity;
        return conjugate() * (1.0f / ls);
    }

    float dot(const Quaternion& q) const {
        return _x*q._x + _y*q._y + _z*q._z + _w*q._w;
    }

    Quaternion slerp(const Quaternion& to, float t) const {
        float d = dot(to);
        Quaternion target = to;

        if (d < 0.0f) {
            target = -to;
            d = -d;
        }

        if (d > 0.9995f)
            return (*this + (target - *this) * t).normalized();

        float angle = std::acos(d);
        float sin_angle = std::sin(angle);
        float ta = std::sin((1.0f - t) * angle) / sin_angle;
        float tb = std::sin(t * angle) / sin_angle;
        return *this * ta + target * tb;
    }

    Vector3 rotate(const Vector3& v) const;

    Matrix4 to_matrix() const;
};

inline const Quaternion Quaternion::Identity = {};

inline Quaternion operator*(float s, const Quaternion& q) {
    return q * s;
}

inline Vector3 Quaternion::rotate(const Vector3& v) const {
    Quaternion p = { v.x, v.y, v.z, 0.0f };
    Quaternion r = *this * p * inversed();
    return { r._x, r._y, r._z };
}

inline Matrix4 Quaternion::to_matrix() const {
    Quaternion q = normalized();
    float xx = q._x*q._x, yy = q._y*q._y, zz = q._z*q._z;
    float xy = q._x*q._y, xz = q._x*q._z, yz = q._y*q._z;
    float wx = q._w*q._x, wy = q._w*q._y, wz = q._w*q._z;

    Matrix4 r = Matrix4::Identity;
    r[0][0] = 1 - 2*(yy+zz);  r[0][1] = 2*(xy-wz);      r[0][2] = 2*(xz+wy);
    r[1][0] = 2*(xy+wz);      r[1][1] = 1 - 2*(xx+zz);  r[1][2] = 2*(yz-wx);
    r[2][0] = 2*(xz-wy);      r[2][1] = 2*(yz+wx);      r[2][2] = 1 - 2*(xx+yy);
    return r;
}

inline Quaternion Quaternion::from_matrix(const Matrix4& mat) {
    float trace = mat[0][0] + mat[1][1] + mat[2][2];

    if (trace > 0.0f) {
        float s = 0.5f / std::sqrt(trace + 1.0f);
        return {
            (mat[2][1] - mat[1][2]) * s,
            (mat[0][2] - mat[2][0]) * s,
            (mat[1][0] - mat[0][1]) * s,
            0.25f / s
        };
    } else if (mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2]) {
        float s = 2.0f * std::sqrt(1.0f + mat[0][0] - mat[1][1] - mat[2][2]);
        return {
            0.25f * s,
            (mat[0][1] + mat[1][0]) / s,
            (mat[0][2] + mat[2][0]) / s,
            (mat[2][1] - mat[1][2]) / s
        };
    } else if (mat[1][1] > mat[2][2]) {
        float s = 2.0f * std::sqrt(1.0f + mat[1][1] - mat[0][0] - mat[2][2]);
        return {
            (mat[0][1] + mat[1][0]) / s,
            0.25f * s,
            (mat[1][2] + mat[2][1]) / s,
            (mat[0][2] - mat[2][0]) / s
        };
    } else {
        float s = 2.0f * std::sqrt(1.0f + mat[2][2] - mat[0][0] - mat[1][1]);
        return {
            (mat[0][2] + mat[2][0]) / s,
            (mat[1][2] + mat[2][1]) / s,
            0.25f * s,
            (mat[1][0] - mat[0][1]) / s
        };
    }
}

namespace std {

template<>
struct hash<Quaternion> {
    size_t operator()(const Quaternion& q) const noexcept {
        return hash<float>{}(q._x) ^
            (hash<float>{}(q._y) << 1) ^
            (hash<float>{}(q._z) << 2) ^
            (hash<float>{}(q._w) << 3);
    }
};

}

template<>
struct std::formatter<Quaternion::EulerAngles::Proxy>
    : std::formatter<float>
{
    auto format(const Quaternion::EulerAngles::Proxy& p,
                std::format_context& ctx) const
    {
        return std::formatter<float>::format(
            static_cast<float>(p),
            ctx
        );
    }
};
