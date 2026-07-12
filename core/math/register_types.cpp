#include "core/math/color.h"
#include "core/math/matrix3.h"
#include "core/math/matrix4.h"
#include "core/math/vector2.h"
#include "core/math/vector2i.h"
#include "core/math/vector3.h"
#include "core/math/vector3i.h"
#include "core/math/quaternion.h"
#include "core/math/vector4.h"
#include "tools/reflector/type_registry.h"
#include <format>
#include <string>

template<size_t Rows, size_t Cols>
std::string matrix_to_string(const float (&m)[Rows][Cols]) {
    std::string s = "[";
    for (size_t i = 0; i < Rows; ++i) {
        if (i) s += ", ";
        s += "[";
        for (size_t j = 0; j < Cols; ++j) {
            if (j) s += ", ";
            s += std::format("{}", m[i][j]);
        }
        s += "]";
    }
    s += "]";
    return s;
}

const bool math_registered = [] {
    register_type<Vector2>(
        "Vector2",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector2*>(p);
            return std::format("{},{}", v.x, v.y);
        },
        TypeKind::Class
    );
    
    register_type<Vector3>(
        "Vector3",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector3*>(p);
            return std::format("{},{},{}", v.x, v.y, v.z);
        },
        TypeKind::Class
    );

    register_type<Vector4>(
        "Vector4",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector4*>(p);
            return std::format("{},{},{},{}", v.x, v.y, v.z, v.w);
        },
        TypeKind::Class
    );
    
    register_type<Vector2i>(
        "Vector2i",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector2i*>(p);
            return std::format("{},{}", v.x, v.y);
        },
        TypeKind::Class
    );
    
    register_type<Vector3i>(
        "Vector3i",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector3i*>(p);
            return std::format("{},{},{}", v.x, v.y, v.z);
        },
        TypeKind::Class
    );

    register_type<Quaternion>(
        "Quaternion",
        [](const void* p) -> std::string {
            const auto& q = *static_cast<const Quaternion*>(p);
            return std::format("{},{},{},{}", q._x, q._y, q._z, q._w);
        },
        TypeKind::Class
    );

    register_type<Color>(
        "Color",
        [](const void* p) -> std::string {
            const auto& c = *static_cast<const Color*>(p);
            return std::format("{},{},{},{}", c.r, c.g, c.b, c.a);
        },
        TypeKind::Class
    );

    register_type<Matrix4>(
        "Matrix4",
        [](const void* p) {
            return matrix_to_string(static_cast<const Matrix4*>(p)->m);
        }
    );

    register_type<Matrix3>(
        "Matrix3",
        [](const void* p) {
            return matrix_to_string(static_cast<const Matrix3*>(p)->m);
        }
    );

    return true;
}();
