#pragma once
#include "core/math/matrix4.h"
#include "render_target.h"

struct ViewPort {
    ViewPort(int width = 1280, int height = 720, int x = 0, int y = 0, RenderTarget* target = nullptr)
        : width(width), height(height), x(x), y(y), target(target) {}

    int width  = 1280;
    int height = 720;
    int x = 0, y = 0;

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
