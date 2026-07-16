#pragma once
#include "render/renderer.h"

class Scene;

class System {
public:
    virtual ~System() = default;
    
    virtual void awake(Scene& scene) {};
    virtual void start(Scene& scene) {};
    virtual void update(Scene& scene) {};
    virtual void draw(Scene& scene, ViewPort& viewport, RenderData& render_data) {};

private:
    bool started = false;
    
    void update_internal(Scene& scene) {
        if (!started) {
            start(scene);
            started = true;
        }
        update(scene);
    }

    friend class Scene;
};
