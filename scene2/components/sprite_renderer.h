#pragma once
#include "scene2/component.h"
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

