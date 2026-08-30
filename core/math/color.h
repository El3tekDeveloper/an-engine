#pragma once
#include "core/math/vector_convert.h"
#include <cmath>
#include <functional>
#include <stdexcept>

struct [[nodiscard]] alignas(16) Color {
    static const Color White;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;

    float r, g, b, a;

    constexpr Color(float r, float g, float b, float a = 1.0f)
        : r(r), g(g), b(b), a(a) {}

    template<typename C> requires vecconv::ConvertibleFrom<C, Color>
    constexpr Color(const C& c)
    : r(static_cast<float>(vecconv::comp_x(c))),
      g(static_cast<float>(vecconv::comp_y(c))),
      b(static_cast<float>(vecconv::comp_z(c, 1.0f))),
      a(static_cast<float>(vecconv::comp_w(c, 1.0f))) {};

    Color(const char* hex) : r(0), g(0), b(0), a(1.0f) {
        if (!hex || hex[0] != '#')
            throw std::invalid_argument("Color: hex string must start with '#'");

        hex++;

        auto parseHex = [](char c) -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            throw std::invalid_argument("Color: invalid hex character");
        };

        auto parseByte = [&](int i) -> float {
            return (float)((parseHex(hex[i]) << 4) | parseHex(hex[i + 1])) / 255.0f;
        };

        auto srgbToLinear = [](float c) -> float {
            return c <= 0.04045f ? c / 12.92f : powf((c + 0.055f) / 1.055f, 2.4f);
        };

        int len = 0;
        while (hex[len]) len++;

        if (len == 6) {
            r = srgbToLinear(parseByte(0));
            g = srgbToLinear(parseByte(2));
            b = srgbToLinear(parseByte(4));
        } else if (len == 8) {
            r = srgbToLinear(parseByte(0));
            g = srgbToLinear(parseByte(2));
            b = srgbToLinear(parseByte(4));
            a = parseByte(6);
        } else {
            throw std::invalid_argument("Color: hex string must be #RRGGBB or #RRGGBBAA");
        }
    }
};

namespace std {

template<>
struct hash<Color> {
    size_t operator()(const Color& c) const noexcept {
        return hash<float>{}(c.r) ^
            (hash<float>{}(c.g) << 1) ^
            (hash<float>{}(c.b) << 2) ^
            (hash<float>{}(c.a) << 3);
    }
};

}

inline const Color Color::White = {1.0f, 1.0f, 1.0f, 1.0f};
inline const Color Color::Black = {0.0f, 0.0f, 0.0f, 1.0f};
inline const Color Color::Red   = {1.0f, 0.0f, 0.0f, 1.0f};
inline const Color Color::Green = {0.0f, 1.0f, 0.0f, 1.0f};
inline const Color Color::Blue  = {0.0f, 0.0f, 1.0f, 1.0f};
