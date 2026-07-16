#pragma once
#include "core/math/color.h"
#include "core/math/matrix4.h"
#include "resources/recource.h"
#include "resources/texture.h"
#include "resources/resource_manager.h"
#include "core/math/vector2.h"
#include "core/utils/uuid.h"
#include "core/debug/logger.h"

struct SpriteRect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

class Sprite : public Resource {
public:
    Sprite() = default;
    explicit Sprite(Texture* texture) : texture(texture) {
        if (texture) {
            rect = { 0, 0, texture->get_width(), texture->get_height() };
        }
    }
    virtual ~Sprite() = default;

    bool load(const std::string& file_path) {
        Texture* tex = ResourceManager.load<Texture>(file_path);
        if (!tex) {
            LOG_ERROR("Failed to load sprite texture: '{}'", file_path.c_str());
            return false;
        }

        texture = tex;
        rect = { 0, 0, texture->get_width(), texture->get_height() };
        uuid = make_uuid(file_path);
        return true;
    }
    
    bool is_valid() const {
        return texture && texture->is_valid();
    }

    Vector2 get_size() const {
        return Vector2(
            (float)rect.width / pixels_per_unit,
            (float)rect.height / pixels_per_unit
        );
    }

    void get_uv_rect(Vector2& uv_min, Vector2& uv_max) const {
        if (!texture || texture->get_width() <= 0 || texture->get_height() <= 0) {
            uv_min = Vector2(0.0f, 0.0f);
            uv_max = Vector2(1.0f, 1.0f);
            return;
        }

        float tex_width = (float)texture->get_width();
        float tex_height = (float)texture->get_height();

        uv_min = Vector2(rect.x / tex_width, rect.y / tex_height);
        uv_max = Vector2((rect.x + rect.width) / tex_width, (rect.y + rect.height) / tex_height);
    }

    Texture* texture = nullptr;
    SpriteRect rect;

    Vector2 pivot = Vector2(0.5f, 0.5f);

    float pixels_per_unit = 100.0f;
};

struct SpriteInstance {
    const Sprite* sprite = nullptr;
    Matrix4 model = Matrix4::Identity;
    Color color = Color::White;
    bool flip_x = false;
    bool flip_y = false;
    int layer = 0;
};
