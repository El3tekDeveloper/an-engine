#pragma once
#include "scene2/ecs.h"
#include "scene2/components/transform.h"
#include "scene2/scene.h"

using namespace ecs;

struct GameObject {
    Entity id;
    Scene* scene;
    
    inline void destroy() {
        scene->destroy(id);
    }

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

    inline void set_parent(GameObject parent) const {
        scene->set_parent(id, parent.id);
    }
 
    inline void remove_parent() const {
        scene->remove_parent(id);
    }
 
    inline GameObject get_parent() const {
        return GameObject{ scene->get_parent(id), scene };
    }
 
    inline bool has_parent() const {
        return entity::is_valid(scene->get_parent(id));
    }
 
    inline std::vector<GameObject> get_children() const {
        std::vector<GameObject> result;
        for (Entity child : scene->get_children(id))
            result.push_back(GameObject{ child, scene });
        return result;
    }
 
    template<typename Fn>
    inline void each_child(Fn&& fn, bool recursive = true) const {
        scene->each_child(id, [&](Entity child) {
            fn(GameObject{ child, scene });
        }, recursive);
    }
};
