// Generated reflection for: Panel
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./scene2/components/panel.h"
#include "scene2/components/component_ops.h"

template<>
inline TypeClass* get_class_impl<Panel>() {
    static TypeClass c("Panel", sizeof(Panel));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<Panel>(); };
    c.create_instance = []() -> void* { return new Panel(); };

    c.parent_type = static_cast<TypeClass*>(get_type_by_name("Component"));


    component_ops_registry()[&c] = ComponentTypeOps{
        [](Scene& scene, Entity id) -> Component* {
            return &scene.assign_component<Panel>(id);
        },
        [](Scene& scene, Entity id) -> Component* {
            return &scene.get_component<Panel>(id);
        },
        [](Scene& scene, Entity id) -> bool {
            return scene.get_entities()[entity::index(id)].mask.test(type_id<Panel>());
        },
        [](Scene& scene, Entity id) -> void {
            scene.remove_component<Panel>(id);
        },
    };

    return &c;
}

namespace {
    struct Panel_AutoRegister {
        Panel_AutoRegister() {
            register_type<Panel>(get_class_impl<Panel>());
        }
    };
    inline Panel_AutoRegister Panel_auto_register_instance;
}


