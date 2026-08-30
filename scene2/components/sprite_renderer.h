#pragma once
#include "component.h"
#include "resources/sprite.h"

struct [[export]] SpriteRenderer : public Component {
    SpriteRenderer() = default;
    explicit SpriteRenderer(Sprite& sprite) : sprite(&sprite) {}
 
    [[export]] Sprite* sprite = nullptr;
    [[export]] Color color = Color::White;
 
    [[export]] bool flip_x = false;
    [[export]] bool flip_y = false;
 
    [[export]] int layer = 0;
};

#if __REFLECT_GENERATED__
#include ".generated/sprite_renderer.generated.hxx"
#endif
