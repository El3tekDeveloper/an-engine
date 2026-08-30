#pragma once
#include "ecs.h"
#include "render/renderer.h"
#include "resources/script.h"
#include "components/script_component.h"
#include <cassert>
#include <memory>
#include <vector>

using namespace ecs;
namespace ecs { class Registry; }

struct GameObject;
class System;
class Script;

struct EntityRecord {
    Entity id = entity::INVALID;
    ComponentMask mask;
    
    Entity parent = entity::INVALID;
    std::vector<Entity> children;
};

class Scene {
public:
    Scene() = default;
    virtual ~Scene() = default;
    
    void initialize();
    void update();
    void draw(ViewPort& viewport, RenderData& render_data);
    
    void add_system(std::unique_ptr<System> system);
    template<typename T>
    T& get_system();

    void print_scene_tree() const;
    void save(const std::string& path);

    Entity create_entity();  
    void destroy(Entity id);

    GameObject create_game_object();
    void destroy(GameObject game_object);

    const std::vector<EntityRecord>&
        get_entities() const { return entities; }

    template<typename T, typename... TArgs>
    T& assign_component(Entity id, TArgs&&... args);

    template<typename T>
    T& get_component(Entity id);
    
    template<typename T>
    void remove_component(Entity id);
    
    void set_parent(Entity child, Entity parent);
    void remove_parent(Entity child);
    Entity get_parent(Entity child) const;
    const std::vector<Entity>& get_children(Entity parent) const;
 
    template<typename Fn>
    void each_child(Entity parent, Fn&& fn, bool recursive = true) const;

    template<typename T, typename... Args>
    Script& add_script(Entity entity, Args&&... args);

    ecs::Registry& get_registry() const { return *registry; }

private:
    bool initialized = false;
    
    std::vector<EntityRecord> entities;
    std::vector<Entity> free_entities;
    std::vector<std::unique_ptr<ComponentPool>> component_pools;

    ecs::Registry* registry;
    std::vector<std::unique_ptr<System>> systems;
};

template<typename T>
T& Scene::get_system() {
    for (auto& system : systems) {
        if (auto* ptr = dynamic_cast<T*>(system.get())) {
            return *ptr;
        }
    }
}

template<typename T, typename... TArgs>
T& Scene::assign_component(Entity id, TArgs&&... args) {
    auto component_id = type_id<T>();

    if (component_pools.size() <= component_id)
        component_pools.resize(component_id + 1);

    if (component_pools[component_id] == nullptr)
        component_pools[component_id] = std::make_unique<ComponentPool>(sizeof(T));

    T* raw = new (component_pools[component_id]->get(entity::index(id)))
        T(std::forward<TArgs>(args)...);

    entities[entity::index(id)].mask.set(component_id);
    return *raw;
}

template<typename T>
T& Scene::get_component(Entity id) {
    auto component_id = type_id<T>();
    assert(entities[entity::index(id)].mask.test(component_id)
           && "Scene::get_component: entity does not have this component");

    return *static_cast<T*>(component_pools[component_id]->get(entity::index(id)));
}

template<typename T>
void Scene::remove_component(Entity id) {
    if (entities[entity::index(id)].id != id)
        return;

    entities[entity::index(id)].mask.reset(type_id<T>());
}

template<typename Fn>
void Scene::each_child(Entity parent, Fn&& fn, bool recursive) const {
    for (Entity child : get_children(parent)) {
        fn(child);
        if (recursive)
            each_child(child, std::forward<Fn>(fn), true);
    }
}

template<typename T, typename... Args>
inline Script& Scene::add_script(Entity entity, Args&&... args) {
    auto& comp = assign_component<ScriptComponent>(
        entity, ScriptComponent::create<T>(std::forward<Args>(args)...)
    );
    Script* script = comp.instance.get();

    if (initialized) {
        script->awake(*this, entity);
        script->awoken = true;
    }
    return *script;
}
