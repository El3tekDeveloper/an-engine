// using Claude AI to save time
#pragma once
#include <cmath>
#include "vector3.h"

struct [[nodiscard]] Matrix4 {
    float m[4][4]{};

    constexpr Matrix4() = default;

    constexpr Matrix4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    constexpr explicit Matrix4(float diagonal) {
        for (int i = 0; i < 4; ++i)
            m[i][i] = diagonal;
    }

    constexpr explicit Matrix4(const float* value) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = value[i * 4 + j];
    }

    static const Matrix4 Identity;
    static const Matrix4 Zero;

    float* operator[](int row) {
        return m[row];
    }

    const float* operator[](int row) const {
        return m[row];
    }
    
    Matrix4 operator+(const Matrix4& o) const {
        Matrix4 r;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r.m[i][j] = m[i][j] + o.m[i][j];
        return r;
    }

    Matrix4 operator-(const Matrix4& o) const {
        Matrix4 r;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r.m[i][j] = m[i][j] - o.m[i][j];
        return r;
    }

    Matrix4 operator*(const Matrix4& o) const {
        Matrix4 r;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                r.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k)
                    r.m[i][j] += m[i][k] * o.m[k][j];
            }
        return r;
    }

    Matrix4 operator*(float s) const {
        Matrix4 r;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r.m[i][j] = m[i][j] * s;
        return r;
    }

    Matrix4 operator/(float s) const {
        return *this * (1.0f / s);
    }

    Matrix4& operator+=(const Matrix4& o) { return *this = *this + o; }
    Matrix4& operator-=(const Matrix4& o) { return *this = *this - o; }
    Matrix4& operator*=(const Matrix4& o) { return *this = *this * o; }
    Matrix4& operator*=(float s)          { return *this = *this * s; }
    Matrix4& operator/=(float s)          { return *this = *this / s; }

    bool operator==(const Matrix4& o) const {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (m[i][j] != o.m[i][j]) return false;
        return true;
    }

    bool operator!=(const Matrix4& o) const {
        return !(*this == o);
    }

    const float* data() const {
        return &m[0][0];
    }

    Matrix4 transposed() const {
        Matrix4 r;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r.m[i][j] = m[j][i];
        return r;
    }

    float determinant() const {
        float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3];

        auto det3 = [](float a, float b, float c,
                       float d, float e, float f,
                       float g, float h, float k) -> float {
            return a*(e*k - f*h) - b*(d*k - f*g) + c*(d*h - e*g);
        };

        float c0 = +det3(m[1][1], m[1][2], m[1][3],
                         m[2][1], m[2][2], m[2][3],
                         m[3][1], m[3][2], m[3][3]);

        float c1 = -det3(m[1][0], m[1][2], m[1][3],
                         m[2][0], m[2][2], m[2][3],
                         m[3][0], m[3][2], m[3][3]);

        float c2 = +det3(m[1][0], m[1][1], m[1][3],
                         m[2][0], m[2][1], m[2][3],
                         m[3][0], m[3][1], m[3][3]);

        float c3 = -det3(m[1][0], m[1][1], m[1][2],
                         m[2][0], m[2][1], m[2][2],
                         m[3][0], m[3][1], m[3][2]);

        return a00*c0 + a01*c1 + a02*c2 + a03*c3;
    }

    Matrix4 inversed() const {
        auto det3 = [](float a, float b, float c,
                       float d, float e, float f,
                       float g, float h, float k) -> float {
            return a*(e*k - f*h) - b*(d*k - f*g) + c*(d*h - e*g);
        };

        Matrix4 adj;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                float sub[3][3];
                int si = 0;
                for (int r = 0; r < 4; ++r) {
                    if (r == j) continue;
                    int sj = 0;
                    for (int c = 0; c < 4; ++c) {
                        if (c == i) continue;
                        sub[si][sj++] = m[r][c];
                    }
                    ++si;
                }
                float cofactor = det3(sub[0][0], sub[0][1], sub[0][2],
                                      sub[1][0], sub[1][1], sub[1][2],
                                      sub[2][0], sub[2][1], sub[2][2]);
                adj.m[i][j] = ((i + j) % 2 == 0 ? 1.0f : -1.0f) * cofactor;
            }

        float det = m[0][0]*adj.m[0][0] + m[0][1]*adj.m[1][0]
                  + m[0][2]*adj.m[2][0] + m[0][3]*adj.m[3][0];

        if (det == 0.0f)
            return Zero;

        return adj * (1.0f / det);
    }

    static Matrix4 translation(float tx, float ty, float tz) {
        Matrix4 r = Identity;
        r.m[0][3] = tx;
        r.m[1][3] = ty;
        r.m[2][3] = tz;
        return r;
    }

    static Matrix4 scale(float sx, float sy, float sz) {
        Matrix4 r = Identity;
        r.m[0][0] = sx;
        r.m[1][1] = sy;
        r.m[2][2] = sz;
        return r;
    }

    static Matrix4 scale(float s) {
        return scale(s, s, s);
    }

    static Matrix4 rotation(float angle, float ax, float ay, float az) {
        float c  = std::cos(angle);
        float s  = std::sin(angle);
        float ic = 1.0f - c;

        return {
             c + ax*ax*ic, ax*ay*ic - az*s, ax*az*ic + ay*s, 0.0f,
             ay*ax*ic + az*s, c + ay*ay*ic, ay*az*ic - ax*s, 0.0f,
             az*ax*ic - ay*s, az*ay*ic + ax*s, c + az*az*ic, 0.0f,
             0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    static Matrix4 translation(const Vector3& v) {
        return translation(v.x, v.y, v.z);
    }

    static Matrix4 scale(const Vector3& v) {
        return scale(v.x, v.y, v.z);
    }
    
    static Matrix4 rotation(float angle, const Vector3& v) {
        return rotation(angle, v.x, v.y, v.z);
    }

    static Matrix4 rotation_x(float angle) {
        float c = std::cos(angle), s = std::sin(angle);
        return {
            1,  0,  0,  0,
            0,  c, -s,  0,
            0,  s,  c,  0,
            0,  0,  0,  1
        };
    }

    static Matrix4 rotation_y(float angle) {
        float c = std::cos(angle), s = std::sin(angle);
        return {
             c,  0,  s,  0,
             0,  1,  0,  0,
            -s,  0,  c,  0,
             0,  0,  0,  1
        };
    }

    static Matrix4 rotation_z(float angle) {
        float c = std::cos(angle), s = std::sin(angle);
        return {
            c, -s,  0,  0,
            s,  c,  0,  0,
            0,  0,  1,  0,
            0,  0,  0,  1
        };
    }

    static Matrix4 perspective(float fov_y, float aspect, float z_near, float z_far) {
        float tan_half = std::tan(fov_y * 0.5f);
        float range    = z_near - z_far;

        return {
            1.0f / (aspect * tan_half), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / tan_half, 0.0f, 0.0f,
            0.0f, 0.0f, (z_near + z_far) / range, 2.0f * z_far * z_near / range,
            0.0f, 0.0f, -1.0f, 0.0f
        };
    }

    static Matrix4 orthographic(
        float left, float right,
        float bottom, float top,
        float z_near, float z_far)
    {
        float rml = right - left;
        float tmb = top   - bottom;
        float fmn = z_far - z_near;

        return {
            2.0f / rml, 0.0f, 0.0f, -(right + left) / rml,
            0.0f, 2.0f / tmb,  0.0f, -(top   + bottom) / tmb,
            0.0f, 0.0f, -2.0f / fmn, -(z_far + z_near) / fmn,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    static Matrix4 look_at(
        float eye_x,    float eye_y,    float eye_z,
        float center_x, float center_y, float center_z,
        float up_x,     float up_y,     float up_z)
    {
        float fx = eye_x - center_x, fy = eye_y - center_y, fz = eye_z - center_z;
        float fl = std::sqrt(fx*fx + fy*fy + fz*fz);
        fx /= fl; fy /= fl; fz /= fl;

        float rx = up_y*fz - up_z*fy;
        float ry = up_z*fx - up_x*fz;
        float rz = up_x*fy - up_y*fx;
        float rl = std::sqrt(rx*rx + ry*ry + rz*rz);
        rx /= rl; ry /= rl; rz /= rl;

        float ux = fy*rz - fz*ry;
        float uy = fz*rx - fx*rz;
        float uz = fx*ry - fy*rx;

        return {
             rx,  ry,  rz, -(rx*eye_x + ry*eye_y + rz*eye_z),
             ux,  uy,  uz, -(ux*eye_x + uy*eye_y + uz*eye_z),
             fx,  fy,  fz, -(fx*eye_x + fy*eye_y + fz*eye_z),
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    Vector3 transform_point(const Vector3& v) const;

    Vector3 transform_direction(const Vector3& v) const;
};

inline const Matrix4 Matrix4::Identity = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

inline const Matrix4 Matrix4::Zero = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

inline Matrix4 operator*(float s, const Matrix4& mat) {
    return mat * s;
}

inline Vector3 Matrix4::transform_point(const Vector3& v) const {
    return {
        m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3],
        m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3],
        m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]
    };
}

inline Vector3 Matrix4::transform_direction(const Vector3& v) const {
    return {
        m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
        m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
        m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
    };
}
