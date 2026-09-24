#pragma once
#include "core/math/color.h"
#include "core/math/matrix4.h"
#include "core/math/vector4.h"
#include "resources/gpu_types.h"
#include "resources/resource.h"
#include <sys/stat.h>

class [[export]] StyleBox : public Resource {
public:
    virtual ~StyleBox() = default;
    
    [[export]]
    Color bg_color = Color::White;
    
    [[export]]
    Vector4 border_width = Vector4::One;
    [[export]]
    Color border_color = Color::Black;

    [[export]]
    Vector4 corner_radius = Vector4::Zero;
    [[export]]
    Vector4 expand_margins = Vector4::Zero;
    
    static constexpr size_t gpu_size() {
        return sizeof(StyleBox) - offsetof(StyleBox, bg_color);
    }

    static const GpuHandle& get_gpu_handle() {
        static GpuHandle handle = [] {
            GpuHandle h("StyleBox");
            ADD_GPU_FIELD(h, StyleBox, bg_color);
            ADD_GPU_FIELD(h, StyleBox, border_width);
            ADD_GPU_FIELD(h, StyleBox, border_color);
            ADD_GPU_FIELD(h, StyleBox, corner_radius);
            ADD_GPU_FIELD(h, StyleBox, expand_margins);
            return h;
        }();
        return handle;
    }
};

struct PanelInstance {
    StyleBox* panel;
    Matrix4 model = Matrix4::Identity;
};

