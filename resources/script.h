#pragma once
#include "scene2/ecs.h"
#include "render/renderer.h"

using namespace ecs;
class Scene;
struct Component;

class Script : public Resource {
public:
    virtual ~Script() = default;

    virtual void awake(Scene& scene, Entity entity) {}
    virtual void start(Scene& scene, Entity entity) {}
    virtual void update(Scene& scene, Entity entity) {}
    virtual void draw(Scene& scene, Entity entity, RenderData& rd) {}

    bool awoken = false;
    bool started = false;
};
