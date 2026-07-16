#pragma once
#include "core/math/matrix4.h"
#include "render_target.h"

struct ViewPort {
    int x = 0, y = 0;
    int width  = 1280;
    int height = 720;

    RenderTarget* target = nullptr;

    void resize(int new_width, int new_height) {
        width = new_width;
        height = new_height;
        if (target) target->resize(width, height);
    }

    float aspect_ratio() const {
        return height > 0 ? (float)width / (float)height : 1.0f;
    }
};

struct CameraData {
    Matrix4 matrix = Matrix4::Identity;
};
