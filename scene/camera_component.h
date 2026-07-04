#pragma once
#include "core/math/matrix4.h"
#include "tools/reflector/type_registry.h"
#include "behaviour.h"
#include "game_object.h"
#include "render/renderer.h"

class [[export]] Camera : public Behaviour {
public:
    Camera() = default;
    ~Camera() = default;
    
    [[export, range(1, 179)]]
    float fov = 60.0f;
    [[export, range(0.001, 100)]]
    float near_plane = 0.1f;
    [[export, range(1, 10000)]]
    float far_plane = 100.0f;

    void draw(RenderData& rd) override {
        view = game_object().transform.get_matrix().inversed();

        projection = Matrix4::perspective(
            to_radians(fov),
            (float)camera_data.viewport.width / (float)camera_data.viewport.height,
            near_plane,
            far_plane
        );
        
        camera_data.matrix = projection * view;
        rd.camera = &camera_data;
    }

    [[export]] Matrix4 get_projection() const { return projection; }
    [[export]] Matrix4 get_view() const { return view; }
    
    CameraData* get_camera_data() { return &camera_data; }
    Matrix4 get_matrix() const { return camera_data.matrix; }
    [[export]] ViewPort get_viewport() const { return camera_data.viewport; }

private:
    Matrix4 view;
    Matrix4 projection;
    CameraData camera_data;
};

#if __REFLECT_GENERATED__
#include ".generated/camera_component.generated.hxx"
#endif
