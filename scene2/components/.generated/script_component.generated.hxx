// Generated reflection for: ScriptComponent
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./scene2/components/script_component.h"
#include "scene2/components/component_ops.h"

template<>
inline TypeClass* get_class_impl<ScriptComponent>() {
    static TypeClass c("ScriptComponent", sizeof(ScriptComponent));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<ScriptComponent>(); };
    c.create_instance = []() -> void* { return new ScriptComponent(); };

    c.parent_type = static_cast<TypeClass*>(get_type_by_name("Component"));


    component_ops_registry()[&c] = ComponentTypeOps{
        [](Scene& scene, Entity id) -> Component* {
            return &scene.assign_component<ScriptComponent>(id);
        },
        [](Scene& scene, Entity id) -> Component* {
            return &scene.get_component<ScriptComponent>(id);
        },
        [](Scene& scene, Entity id) -> bool {
            return scene.get_entities()[entity::index(id)].mask.test(type_id<ScriptComponent>());
        },
        [](Scene& scene, Entity id) -> void {
            scene.remove_component<ScriptComponent>(id);
        },
    };

    return &c;
}

namespace {
    struct ScriptComponent_AutoRegister {
        ScriptComponent_AutoRegister() {
            register_type<ScriptComponent>(get_class_impl<ScriptComponent>());
        }
    };
    inline ScriptComponent_AutoRegister ScriptComponent_auto_register_instance;
}


