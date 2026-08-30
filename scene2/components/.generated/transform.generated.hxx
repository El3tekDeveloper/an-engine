// Generated reflection for: Transform
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./scene2/components/transform.h"
#include "scene2/components/component_ops.h"

template<>
inline TypeClass* get_class_impl<Transform>() {
    static TypeClass c("Transform", sizeof(Transform));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<Transform>(); };
    c.create_instance = []() -> void* { return new Transform(); };

    c.parent_type = static_cast<TypeClass*>(get_type_by_name("Component"));

    {
        static const std::string_view _fname_position = "position";
        static const std::string_view _ftype_position = "Vector3";
        Field f;
        f.name = _fname_position;
        f.offset = offsetof(Transform, position);
        f.type = get_type<Vector3>();
        f.type->set_name(_ftype_position);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_rotation = "rotation";
        static const std::string_view _ftype_rotation = "Quaternion";
        Field f;
        f.name = _fname_rotation;
        f.offset = offsetof(Transform, rotation);
        f.type = get_type<Quaternion>();
        f.type->set_name(_ftype_rotation);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_scale = "scale";
        static const std::string_view _ftype_scale = "Vector3";
        Field f;
        f.name = _fname_scale;
        f.offset = offsetof(Transform, scale);
        f.type = get_type<Vector3>();
        f.type->set_name(_ftype_scale);
        c.get_fields().push_back(f);
    }

    component_ops_registry()[&c] = ComponentTypeOps{
        [](Scene& scene, Entity id) -> Component* {
            return &scene.assign_component<Transform>(id);
        },
        [](Scene& scene, Entity id) -> Component* {
            return &scene.get_component<Transform>(id);
        },
        [](Scene& scene, Entity id) -> bool {
            return scene.get_entities()[entity::index(id)].mask.test(type_id<Transform>());
        },
        [](Scene& scene, Entity id) -> void {
            scene.remove_component<Transform>(id);
        },
    };

    return &c;
}

namespace {
    struct Transform_AutoRegister {
        Transform_AutoRegister() {
            register_type<Transform>(get_class_impl<Transform>());
        }
    };
    inline Transform_AutoRegister Transform_auto_register_instance;
}


