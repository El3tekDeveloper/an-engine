// Generated reflection for: Camera
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "scene/camera_component.h"

template<>
inline TypeClass* get_class_impl<Camera>() {
    static TypeClass c("Camera", sizeof(Camera));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<Camera>(); };
    c.create_instance = []() -> void* { return new Camera(); };

    c.parent_type = get_class_by_name("Behaviour");

    {
        static const std::string_view _fname_fov = "fov";
        static const std::string_view _ftype_fov = "float";
        Field f;
        f.name = _fname_fov;
        f.offset = offsetof(Camera, fov);
        f.type = get_type<float>();
        f.type->set_name(_ftype_fov);
        f.has_range = true;
        f.range_min = 1;
        f.range_max = 179;
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
        f.has_range = true;
        f.range_min = 0.001;
        f.range_max = 100;
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
        f.has_range = true;
        f.range_min = 1;
        f.range_max = 10000;
        c.get_fields().push_back(f);
    }
    {
        static const std::string _fnname_get_projection = "get_projection";
        static const std::string _fnret_get_projection = "Matrix4";
        RefFunction fn;
        fn.name = _fnname_get_projection.c_str();
        fn.return_value.name = _fnname_get_projection.c_str();
        fn.return_value.offset = 0;
        fn.return_value.type   = get_type<Matrix4>();
        fn.invoke = [](void* self, void** args, void* ret) {
            auto* obj = static_cast<Camera*>(self);
            *static_cast<Matrix4*>(ret) = obj->get_projection();
        };
        c.get_functions().push_back(fn);
    }
    {
        static const std::string _fnname_get_view = "get_view";
        static const std::string _fnret_get_view = "Matrix4";
        RefFunction fn;
        fn.name = _fnname_get_view.c_str();
        fn.return_value.name = _fnname_get_view.c_str();
        fn.return_value.offset = 0;
        fn.return_value.type   = get_type<Matrix4>();
        fn.invoke = [](void* self, void** args, void* ret) {
            auto* obj = static_cast<Camera*>(self);
            *static_cast<Matrix4*>(ret) = obj->get_view();
        };
        c.get_functions().push_back(fn);
    }
    {
        static const std::string _fnname_get_viewport = "get_viewport";
        static const std::string _fnret_get_viewport = "ViewPort";
        RefFunction fn;
        fn.name = _fnname_get_viewport.c_str();
        fn.return_value.name = _fnname_get_viewport.c_str();
        fn.return_value.offset = 0;
        fn.return_value.type   = get_type<ViewPort>();
        fn.invoke = [](void* self, void** args, void* ret) {
            auto* obj = static_cast<Camera*>(self);
            *static_cast<ViewPort*>(ret) = obj->get_viewport();
        };
        c.get_functions().push_back(fn);
    }

    return &c;
}

namespace {
    struct Camera_AutoRegister {
        Camera_AutoRegister() {
            CLASS_REGISTRY["Camera"] = get_class_impl<Camera>();
            register_type<Camera>("Camera")
;        }
    };
    inline Camera_AutoRegister Camera_auto_register_instance;
}

