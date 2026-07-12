#pragma once
#include "ecs.h"
#include "render/renderer.h"
#include <memory>

using namespace ecs;
class Scene;
struct Component;

class Script {
public:
    virtual ~Script() = default;

    virtual void awake(Scene& scene, Entity entity) {}
    virtual void start(Scene& scene, Entity entity) {}
    virtual void update(Scene& scene, Entity entity) {}
    virtual void draw(Scene& scene, Entity entity, RenderData& rd) {}

    bool awoken = false;
    bool started = false;
};

struct ScriptComponent {
    std::unique_ptr<Script> instance;

    template <typename T, typename... Args>
    static ScriptComponent create(Args&&... args) {
        ScriptComponent c;
        c.instance = std::make_unique<T>(std::forward<Args>(args)...);
        return c;
    }
};
