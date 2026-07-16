#ifdef __cplusplus

#pragma once
#include "core/math/vector2.h" // IWYU pragma: keep
#include "core/math/vector2i.h" // IWYU pragma: keep
#include "core/math/vector3.h" // IWYU pragma: keep
#include "core/math/vector3i.h" // IWYU pragma: keep
#include "core/math/color.h" // IWYU pragma: keep
#include <cstdint> // IWYU pragma: keep

#define GPU_VEC2 alignas(8) Vector2
#define GPU_VEC2i alignas(8) Vector2i

#define GPU_VEC3 alignas(16) Vector3
#define GPU_VEC3i alignas(16) Vector3i

#define GPU_COLOR alignas(16) Color

#define GPU_UINT alignas(4) uint32_t

#define GPU_COLOR_WHITE Color::White
#define GPU_COLOR_BLACK Color::Black

#define GPU_DEFAULT(x) = x

#define GPU_MAKE_VEC2i(a,b) Vector2i(a,b)
#define GPU_MAKE_VEC2(a,b) Vector2(a,b)

#define GPU_MAKE_VEC3i(a,b,c) Vector3i(a,b,c)
#define GPU_MAKE_VEC3(a,b,c) Vector3(a,b,c)

#define GPU_MAKE_COLOR(a,b,c,d) Color(a,b,c,d)

#else

#define GPU_VEC2 vec2
#define GPU_VEC2i ivec2

#define GPU_VEC3 vec3
#define GPU_VEC3i ivec3

#define GPU_COLOR vec4

#define GPU_UINT uint

#define GPU_COLOR_WHITE vec4(1.0, 1.0, 1.0, 1.0)
#define GPU_COLOR_BLACK vec4(0.0, 0.0, 0.0, 1.0)

#define GPU_DEFAULT(x)

#define GPU_MAKE_VEC2i(a,b)
#define GPU_MAKE_VEC2(a,b)

#define GPU_MAKE_VEC3i(a,b,c)
#define GPU_MAKE_VEC3(a,b,c)

#define GPU_MAKE_COLOR(a,b,c,d)

#endif

