#pragma once
#include "component.h"
#include "core/utils/uid.h"
#include "ecs.h"
#include "tools/reflector/type_registry.h"
#include <memory>
#include <vector>

class Entity {
public:
    Entity() {};
    virtual ~Entity() = default;

    template<typename T, typename... TArgs>
    T& add_component(TArgs&&... args) {
        static_assert(std::is_base_of<Component, T>::value);

        auto comp = std::make_unique<T>(std::forward<TArgs>(args)...);

        T* raw = comp.get();
        raw->entity = this;

        auto id = ECS::type_id<T>();
        component_list[id] = raw;
        mask.set(id);

        components.emplace_back(std::move(comp));

        on_component_added(raw);

        return *raw;
    }
    
    Component* add_component(const std::string& name) {
        TypeClass* tc = get_class_by_name(name);
        if (!tc || !tc->create_instance) return nullptr;

        Component* comp = static_cast<Component*>(tc->create_instance());
        comp->entity = this;
        components.emplace_back(comp);
        on_component_added(comp);
        return comp;
    }

    Component& add_component(TypeClass* tc) {
        Component* comp = static_cast<Component*>(tc->create_instance());
        comp->entity = this;
        components.emplace_back(comp);
        on_component_added(comp);
        return *comp;
    }
    
    template<typename T>
    T& get_component() {
        auto comp = component_list[ECS::type_id<T>()];
        return *static_cast<T*>(comp);
    }

    template<typename T>
    bool has_component() const {
        auto id = ECS::type_id<T>();
        return mask.test(id);
    }

    template<typename T>
    T* try_get_component() {
        auto id = ECS::type_id<T>();
        if (!mask.test(id)) return nullptr;
        return static_cast<T*>(component_list[id]);
    }

    Component* find_component(const std::string& name) {
        for (auto& comp : components)
            if (comp && comp->get_type_name() == name)
                return comp.get();
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<Component>> components;
    ECS::ComponentList component_list;
    ECS::ComponentBitset mask;
    
    UID uid = UID_INVALID;

    virtual void on_component_added(Component* comp) {};

    friend class Scene;
};
