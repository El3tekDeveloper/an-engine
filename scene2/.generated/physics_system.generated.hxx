// Generated reflection for: AABB
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./scene2/physics_system.h"

template<>
inline TypeClass* get_class_impl<AABB>() {
    static TypeClass c("AABB", sizeof(AABB));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<AABB>(); };
    c.create_instance = []() -> void* { return new AABB(); };

    c.parent_type = get_class_by_name("Component");


    component_ops_registry()[&c] = ComponentTypeOps{
        [](Scene& scene, Entity id) -> Component* {
            return &scene.assign_component<AABB>(id);
        },
        [](Scene& scene, Entity id) -> Component* {
            return &scene.get_component<AABB>(id);
        },
        [](Scene& scene, Entity id) -> bool {
            return scene.get_entities()[entity::index(id)].second.test(type_id<AABB>());
        },
        [](Scene& scene, Entity id) -> void {
            scene.remove_component<AABB>(id);
        },
    };

    return &c;
}

namespace {
    struct AABB_AutoRegister {
        AABB_AutoRegister() {
            CLASS_REGISTRY["AABB"] = get_class_impl<AABB>();
            register_type<AABB>("AABB")
;        }
    };
    inline AABB_AutoRegister AABB_auto_register_instance;
}


// Generated reflection for: RigidBody
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./scene2/physics_system.h"

template<>
inline TypeClass* get_class_impl<RigidBody>() {
    static TypeClass c("RigidBody", sizeof(RigidBody));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<RigidBody>(); };
    c.create_instance = []() -> void* { return new RigidBody(); };

    c.parent_type = get_class_by_name("Component");

    {
        static const std::string_view _fname_velocity = "velocity";
        static const std::string_view _ftype_velocity = "Vector3";
        Field f;
        f.name = _fname_velocity;
        f.offset = offsetof(RigidBody, velocity);
        f.type = get_type<Vector3>();
        f.type->set_name(_ftype_velocity);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_mass = "mass";
        static const std::string_view _ftype_mass = "float";
        Field f;
        f.name = _fname_mass;
        f.offset = offsetof(RigidBody, mass);
        f.type = get_type<float>();
        f.type->set_name(_ftype_mass);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_gravity_scale = "gravity_scale";
        static const std::string_view _ftype_gravity_scale = "float";
        Field f;
        f.name = _fname_gravity_scale;
        f.offset = offsetof(RigidBody, gravity_scale);
        f.type = get_type<float>();
        f.type->set_name(_ftype_gravity_scale);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_restitution = "restitution";
        static const std::string_view _ftype_restitution = "float";
        Field f;
        f.name = _fname_restitution;
        f.offset = offsetof(RigidBody, restitution);
        f.type = get_type<float>();
        f.type->set_name(_ftype_restitution);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_is_static = "is_static";
        static const std::string_view _ftype_is_static = "bool";
        Field f;
        f.name = _fname_is_static;
        f.offset = offsetof(RigidBody, is_static);
        f.type = get_type<bool>();
        f.type->set_name(_ftype_is_static);
        c.get_fields().push_back(f);
    }

    component_ops_registry()[&c] = ComponentTypeOps{
        [](Scene& scene, Entity id) -> Component* {
            return &scene.assign_component<RigidBody>(id);
        },
        [](Scene& scene, Entity id) -> Component* {
            return &scene.get_component<RigidBody>(id);
        },
        [](Scene& scene, Entity id) -> bool {
            return scene.get_entities()[entity::index(id)].second.test(type_id<RigidBody>());
        },
        [](Scene& scene, Entity id) -> void {
            scene.remove_component<RigidBody>(id);
        },
    };

    return &c;
}

namespace {
    struct RigidBody_AutoRegister {
        RigidBody_AutoRegister() {
            CLASS_REGISTRY["RigidBody"] = get_class_impl<RigidBody>();
            register_type<RigidBody>("RigidBody")
;        }
    };
    inline RigidBody_AutoRegister RigidBody_auto_register_instance;
}


