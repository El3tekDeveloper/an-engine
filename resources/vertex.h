#ifdef __cplusplus
#pragma once
#include "resources/gpu_types.h"
#endif

struct Vertex {
    GPU_VEC3 position GPU_DEFAULT(GPU_MAKE_VEC3(0,0,0));
    GPU_VEC3 normal   GPU_DEFAULT(GPU_MAKE_VEC3(0,0,0));
    GPU_VEC2 uv       GPU_DEFAULT(GPU_MAKE_VEC2(0,0));
    GPU_COLOR color   GPU_DEFAULT(GPU_COLOR_WHITE);
    GPU_VEC3 tangent  GPU_DEFAULT(GPU_MAKE_VEC3(1,0,0));
};
