#pragma once
#include "scene2/component.h"
#include "core/math/matrix4.h"
#include "core/math/vector2.h"
#include "render/viewport.h"

enum class CameraType {
    Perspective,
    Orthographic
};

struct [[export]] Camera : public Component {
    [[export]] CameraType type = CameraType::Perspective;
    
    [[export, range(1, 179)]]
    float fov = 60.0f;
    
    [[export, range(0.001, 1000)]]
    float orthographic_size = 5.0f;

    [[export, range(0.001, 100)]]
    float near_plane = 0.1f;
    [[export, range(1, 10000)]]
    float far_plane = 100.0f;
    
    static Vector2 screen_to_world(const Vector2& screen);

private:
    inline static Camera* s_active_camera;

    Matrix4 inverse_view_projection;
    ViewPort viewport;

    friend class RenderSystem;
};

inline Vector2 Camera::screen_to_world(const Vector2& screen) {
    const auto& cam = *s_active_camera;

    float x = (2.0f * (screen.x - cam.viewport.x)) / cam.viewport.width - 1.0f;
    float y = 1.0f - (2.0f * (screen.y - cam.viewport.y)) / cam.viewport.height;

    Vector4 clip(x, y, 0.0f, 1.0f);

    Vector4 world = cam.inverse_view_projection * clip;
    world /= world.w;

    return { world.x, world.y };
}

