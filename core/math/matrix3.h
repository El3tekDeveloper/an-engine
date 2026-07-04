#pragma once
#include "matrix4.h"

struct [[nodiscard]] Matrix3 {
    float m[3][3]{};

    constexpr Matrix3() = default;

    constexpr Matrix3(
        float m00, float m01, float m02,
        float m10, float m11, float m12,
        float m20, float m21, float m22)
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
    }

    static const Matrix3 Identity;
    static const Matrix3 Zero;

    const float* data() const { return &m[0][0]; }

    Matrix3 transposed() const {
        Matrix3 r;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                r.m[i][j] = m[j][i];
        return r;
    }

    float determinant() const {
        return m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1])
             - m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0])
             + m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
    }

    Matrix3 inversed() const {
        float det = determinant();
        if (det == 0.0f) return Zero;

        float inv = 1.0f / det;
        Matrix3 r;
        r.m[0][0] =  (m[1][1]*m[2][2] - m[1][2]*m[2][1]) * inv;
        r.m[0][1] = -(m[0][1]*m[2][2] - m[0][2]*m[2][1]) * inv;
        r.m[0][2] =  (m[0][1]*m[1][2] - m[0][2]*m[1][1]) * inv;
        r.m[1][0] = -(m[1][0]*m[2][2] - m[1][2]*m[2][0]) * inv;
        r.m[1][1] =  (m[0][0]*m[2][2] - m[0][2]*m[2][0]) * inv;
        r.m[1][2] = -(m[0][0]*m[1][2] - m[0][2]*m[1][0]) * inv;
        r.m[2][0] =  (m[1][0]*m[2][1] - m[1][1]*m[2][0]) * inv;
        r.m[2][1] = -(m[0][0]*m[2][1] - m[0][1]*m[2][0]) * inv;
        r.m[2][2] =  (m[0][0]*m[1][1] - m[0][1]*m[1][0]) * inv;
        return r;
    }

    static Matrix3 from_matrix4(const Matrix4& mat4) {
        return {
            mat4.m[0][0], mat4.m[0][1], mat4.m[0][2],
            mat4.m[1][0], mat4.m[1][1], mat4.m[1][2],
            mat4.m[2][0], mat4.m[2][1], mat4.m[2][2]
        };
    }
};

inline const Matrix3 Matrix3::Identity = { 1,0,0, 0,1,0, 0,0,1 };
inline const Matrix3 Matrix3::Zero = { 0,0,0, 0,0,0, 0,0,0 };
