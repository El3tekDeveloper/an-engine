#pragma once
#include "system.h"
#include "scene.h"
#include "script.h"
#include "registry.h"

class ScriptSystem : public System {
public:
    virtual ~ScriptSystem() = default;

    void update(Scene& scene) override {
        for (auto entity : scene.get_registry().view<ScriptComponent>()) {
            Script* script = scene.get_component<ScriptComponent>(entity).instance.get();
            if (!script) continue;

            if (!script->started) {
                script->start(scene, entity);
                script->started = true;
            }
            script->update(scene, entity);
        }
    }

    void draw(Scene& scene, ViewPort& viewport, RenderData& rd) override {
        for (auto entity : scene.get_registry().view<ScriptComponent>()) {
            Script* script = scene.get_component<ScriptComponent>(entity).instance.get();
            if (script) script->draw(scene, entity, rd);
        }
    }
};
