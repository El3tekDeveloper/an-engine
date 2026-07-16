#pragma once
#include "core/math/color.h"
#include "core/math/vector3.h"
#include "resources/gpu_types.h"

class Vertex {
public:
    Vertex() = default;
    Vertex(Vector3 position, Vector3 normal, Vector2 uv, Color color = Color::White, Vector3 tangent = Vector3::Right)
        : position(position), normal(normal), uv(uv), color(color), tangent(tangent) {}

    ~Vertex() = default;
    
    Vector3 position = Vector3::Zero;
    Vector3 normal = Vector3::Zero;
    Vector2 uv = Vector2::Zero;
    Color color = Color::White;
    Vector3 tangent = Vector3::Right;

    static const GpuHandle& get_gpu_handle() {
        static GpuHandle handle = [] {
            GpuHandle h("Vertex");
            ADD_GPU_FIELD(h, Vertex, position);
            ADD_GPU_FIELD(h, Vertex, normal);
            ADD_GPU_FIELD(h, Vertex, uv);
            ADD_GPU_FIELD(h, Vertex, color);
            ADD_GPU_FIELD(h, Vertex, tangent);
            return h;
        }();
        return handle;
    }
};
