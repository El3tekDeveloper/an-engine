#pragma once
#include <type_traits>

namespace vecconv {

template <typename V>
concept HasXY = requires(const V& v) { v.x; v.y; };

template <typename V>
concept HasRG = requires(const V& v) { v.r; v.g; };

template <typename V>
concept VectorLike = HasXY<V> || HasRG<V>;

template <VectorLike V>
constexpr auto comp_x(const V& v) {
    if constexpr (HasXY<V>) return v.x;
    else return v.r;
}

template <VectorLike V>
constexpr auto comp_y(const V& v) {
    if constexpr (HasXY<V>) return v.y;
    else return v.g;
}

template <VectorLike V, typename Fallback>
constexpr auto comp_z(const V& v, Fallback fallback) {
    if constexpr (requires { v.z; }) return static_cast<Fallback>(v.z);
    if constexpr (requires { v.b; }) return static_cast<Fallback>(v.b);
    else return fallback;
}

template <VectorLike V, typename Fallback>
constexpr auto comp_w(const V& v, Fallback fallback) {
    if constexpr (requires { v.w; }) return static_cast<Fallback>(v.w);
    if constexpr (requires { v.a; }) return static_cast<Fallback>(v.a);
    else return fallback;
}

template<typename V, typename Self>
concept ConvertibleFrom = VectorLike<V> && !std::is_same_v<std::decay_t<V>, Self>;

}
