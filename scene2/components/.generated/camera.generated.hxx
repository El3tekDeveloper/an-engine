// Generated reflection for: Camera
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./scene2/components/camera.h"
#include "scene2/components/component_ops.h"

template<>
inline TypeClass* get_class_impl<Camera>() {
    static TypeClass c("Camera", sizeof(Camera));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<Camera>(); };
    c.create_instance = []() -> void* { return new Camera(); };

    c.parent_type = static_cast<TypeClass*>(get_type_by_name("Component"));

    {
        static const std::string_view _fname_type = "type";
        static const std::string_view _ftype_type = "CameraType";
        Field f;
        f.name = _fname_type;
        f.offset = offsetof(Camera, type);
        f.type = get_type<CameraType>();
        f.type->set_name(_ftype_type);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_fov = "fov";
        static const std::string_view _ftype_fov = "float";
        Field f;
        f.name = _fname_fov;
        f.offset = offsetof(Camera, fov);
        f.type = get_type<float>();
        f.type->set_name(_ftype_fov);
        f.meta["range_min"] = static_cast<double>(1);
        f.meta["range_max"] = static_cast<double>(179);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_orthographic_size = "orthographic_size";
        static const std::string_view _ftype_orthographic_size = "float";
        Field f;
        f.name = _fname_orthographic_size;
        f.offset = offsetof(Camera, orthographic_size);
        f.type = get_type<float>();
        f.type->set_name(_ftype_orthographic_size);
        f.meta["range_min"] = static_cast<double>(0.001);
        f.meta["range_max"] = static_cast<double>(1000);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_near_plane = "near_plane";
        static const std::string_view _ftype_near_plane = "float";
        Field f;
        f.name = _fname_near_plane;
        f.offset = offsetof(Camera, near_plane);
        f.type = get_type<float>();
        f.type->set_name(_ftype_near_plane);
        f.meta["range_min"] = static_cast<double>(0.001);
        f.meta["range_max"] = static_cast<double>(100);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_far_plane = "far_plane";
        static const std::string_view _ftype_far_plane = "float";
        Field f;
        f.name = _fname_far_plane;
        f.offset = offsetof(Camera, far_plane);
        f.type = get_type<float>();
        f.type->set_name(_ftype_far_plane);
        f.meta["range_min"] = static_cast<double>(1);
        f.meta["range_max"] = static_cast<double>(10000);
        c.get_fields().push_back(f);
    }

    component_ops_registry()[&c] = ComponentTypeOps{
        [](Scene& scene, Entity id) -> Component* {
            return &scene.assign_component<Camera>(id);
        },
        [](Scene& scene, Entity id) -> Component* {
            return &scene.get_component<Camera>(id);
        },
        [](Scene& scene, Entity id) -> bool {
            return scene.get_entities()[entity::index(id)].mask.test(type_id<Camera>());
        },
        [](Scene& scene, Entity id) -> void {
            scene.remove_component<Camera>(id);
        },
    };

    return &c;
}

namespace {
    struct Camera_AutoRegister {
        Camera_AutoRegister() {
            register_type<Camera>(get_class_impl<Camera>());
        }
    };
    inline Camera_AutoRegister Camera_auto_register_instance;
}


