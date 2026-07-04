#pragma once
#include "core/math/color.h"
#include "core/platform/platform.h"
#include "core/memory/bump_allocator.h"
#include "resources/mesh.h"
#include <vector>

enum class ShaderType {
    Fragment,
    Vertex
};

struct ViewPort {
    int width  = 1280;
    int height = 720;

    void resize(int w, int h) {
        width  = w;
        height = h;
    }

    float aspect_ratio() const {
        return height > 0 ? (float)width / (float)height : 1.0f;
    }
};

struct CameraData {
    Matrix4 matrix = Matrix4::Identity;
    ViewPort viewport = {};
};

struct RenderData {
    Color clear_color = Color::Red;
    
    CameraData* camera = nullptr;
    std::vector<MeshInstance> mesh_instances;
};

class Renderer {
public:
    Renderer() {}
    ~Renderer() {
        destroy();
    }

    bool init(Window* window, BumpAllocator* transient_storage);
    void render(RenderData& render_data);
    void resize(int width, int height);
    void destroy();

    bool is_valid() const { return window && window->context; }

private:
    Window* window = nullptr;
    ViewPort* viewport_ptr;
    bool initialized = false;
};
