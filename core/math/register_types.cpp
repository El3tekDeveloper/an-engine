#include "core/math/color.h"
#include "core/math/matrix3.h"
#include "core/math/matrix4.h"
#include "core/math/vector2.h"
#include "core/math/vector2i.h"
#include "core/math/vector3.h"
#include "core/math/vector3i.h"
#include "core/math/quaternion.h"
#include "core/math/vector4.h"
#include "tools/reflector/runtime/type_registry.h"
#include <charconv>
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

template<typename T>
static bool parse_numbers(std::string_view str, T* out, size_t count) {
    size_t found = 0;
    size_t i = 0;

    while (i < str.size() && found < count) {
        char c = str[i];
        if (c == '[' || c == ']' || c == ',' || c == ' ') {
            ++i;
            continue;
        }

        const char* begin = str.data() + i;
        const char* end = str.data() + str.size();
        auto result = std::from_chars(begin, end, out[found]);

        if (result.ec != std::errc())
            return false;

        i += static_cast<size_t>(result.ptr - begin);
        ++found;
    }

    return found == count;
}

const bool math_registered = [] {
    register_type<Vector2>(
        "Vector2",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector2*>(p);
            return std::format("[{},{}]", v.x, v.y);
        },
        [](std::string_view str, void* p) -> bool {
            auto& v = *static_cast<Vector2*>(p);
            float vals[2];
            if (!parse_numbers(str, vals, 2)) return false;
            v.x = vals[0]; v.y = vals[1];
            return true;
        },
        TypeKind::Class
    );

    register_type<Vector3>(
        "Vector3",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector3*>(p);
            return std::format("[{},{},{}]", v.x, v.y, v.z);
        },
        [](std::string_view str, void* p) -> bool {
            auto& v = *static_cast<Vector3*>(p);
            float vals[3];
            if (!parse_numbers(str, vals, 3)) return false;
            v.x = vals[0]; v.y = vals[1]; v.z = vals[2];
            return true;
        },
        TypeKind::Class
    );

    register_type<Vector4>(
        "Vector4",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector4*>(p);
            return std::format("[{},{},{},{}]", v.x, v.y, v.z, v.w);
        },
        [](std::string_view str, void* p) -> bool {
            auto& v = *static_cast<Vector4*>(p);
            float vals[4];
            if (!parse_numbers(str, vals, 4)) return false;
            v.x = vals[0]; v.y = vals[1]; v.z = vals[2]; v.w = vals[3];
            return true;
        },
        TypeKind::Class
    );

    register_type<Vector2i>(
        "Vector2i",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector2i*>(p);
            return std::format("[{},{}]", v.x, v.y);
        },
        [](std::string_view str, void* p) -> bool {
            auto& v = *static_cast<Vector2i*>(p);
            int vals[2];
            if (!parse_numbers(str, vals, 2)) return false;
            v.x = vals[0]; v.y = vals[1];
            return true;
        },
        TypeKind::Class
    );

    register_type<Vector3i>(
        "Vector3i",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const Vector3i*>(p);
            return std::format("[{},{},{}]", v.x, v.y, v.z);
        },
        [](std::string_view str, void* p) -> bool {
            auto& v = *static_cast<Vector3i*>(p);
            int vals[3];
            if (!parse_numbers(str, vals, 3)) return false;
            v.x = vals[0]; v.y = vals[1]; v.z = vals[2];
            return true;
        },
        TypeKind::Class
    );

    register_type<Quaternion>(
        "Quaternion",
        [](const void* p) -> std::string {
            const auto& q = *static_cast<const Quaternion*>(p);
            return std::format("[{},{},{},{}]", q._x, q._y, q._z, q._w);
        },
        [](std::string_view str, void* p) -> bool {
            auto& q = *static_cast<Quaternion*>(p);
            float vals[4];
            if (!parse_numbers(str, vals, 4)) return false;
            q._x = vals[0]; q._y = vals[1]; q._z = vals[2]; q._w = vals[3];
            return true;
        },
        TypeKind::Class
    );

    register_type<Color>(
        "Color",
        [](const void* p) -> std::string {
            const auto& c = *static_cast<const Color*>(p);
            return std::format("[{},{},{},{}]", c.r, c.g, c.b, c.a);
        },
        [](std::string_view str, void* p) -> bool {
            auto& c = *static_cast<Color*>(p);
            float vals[4];
            if (!parse_numbers(str, vals, 4)) return false;
            c.r = vals[0]; c.g = vals[1]; c.b = vals[2]; c.a = vals[3];
            return true;
        },
        TypeKind::Class
    );

    register_type<Matrix4>(
        "Matrix4",
        [](const void* p) {
            return matrix_to_string(static_cast<const Matrix4*>(p)->m);
        },
        [](std::string_view str, void* p) -> bool {
            auto& mat = *static_cast<Matrix4*>(p);
            float vals[16];
            if (!parse_numbers(str, vals, 16)) return false;
            for (size_t i = 0; i < 4; ++i)
                for (size_t j = 0; j < 4; ++j)
                    mat.m[i][j] = vals[i * 4 + j];
            return true;
        }
    );

    register_type<Matrix3>(
        "Matrix3",
        [](const void* p) {
            return matrix_to_string(static_cast<const Matrix3*>(p)->m);
        },
        [](std::string_view str, void* p) -> bool {
            auto& mat = *static_cast<Matrix3*>(p);
            float vals[9];
            if (!parse_numbers(str, vals, 9)) return false;
            for (size_t i = 0; i < 3; ++i)
                for (size_t j = 0; j < 3; ++j)
                    mat.m[i][j] = vals[i * 3 + j];
            return true;
        }
    );

    return true;
}();
