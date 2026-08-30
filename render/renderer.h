#pragma once
#include "core/math/color.h"
#include "core/memory/bump_allocator.h"
#include "resources/mesh.h"
#include "resources/sprite.h"
#include <vector>
#include "resources/style_box.h"
#include "viewport.h"

enum class ShaderType {
    Fragment,
    Vertex
};

struct RenderData {
    Color clear_color = Color::Red;
    CameraData* camera = nullptr;
    
    std::vector<MeshInstance> mesh_instances;
    std::vector<SpriteInstance> sprite_instances;
    std::vector<PanelInstance> panel_instances;
};

class Renderer {
public:
    Renderer() {}
    ~Renderer() { destroy(); }
 
    bool init(BumpAllocator* transient_storage);
    void render(ViewPort& viewport, RenderData& render_data);
    void destroy();
 
    bool is_valid() const { return initialized; }
 
private:
    bool initialized = false;
};
