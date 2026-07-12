#pragma once
#include "scene2/ecs.h"
#include "scene2/render_system.h"
#include "scene2/scene.h"

using namespace ecs;

struct GameObject {
    Entity id;
    Scene* scene;
    
    template<typename T, typename... TArgs>
    inline T& add_component(TArgs&&... args) {
        return scene->assign_component<T>(id, std::forward<TArgs>(args)...);
    }

    template<typename T>
    inline T& get_component() {
        return scene->get_component<T>(id);
    }
    
    template<typename T>
    inline void remove_component() {
        scene->remove_component<T>(id);
    }

    template<typename T, typename... Args>
    inline Script& add_script(Args&&... args) {
        return scene->add_script<T>(id, std::forward<Args>(args)...);
    }

    inline Transform& transform() const {
        return scene->get_component<Transform>(id);
    }
};
