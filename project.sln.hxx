// Generated reflection registry
// DO NOT EDIT
#pragma once
#include "tools/reflector/runtime/type_registry.h"

// @@BEGIN 0//home/el3tek/Projects/C-C++/an-engine/resources/style_box.h
#include "./resources/style_box.h"
template<> struct has_reflection<StyleBox> : std::true_type {};
// @@END

// @@BEGIN 0//home/el3tek/Projects/C-C++/an-engine/scene2/component.h
#include "./scene2/component.h"
template<> struct has_reflection<Component> : std::true_type {};
// @@END

// @@BEGIN 0//home/el3tek/Projects/C-C++/an-engine/scene2/components/camera.h
#include "./scene2/components/camera.h"
template<> struct has_reflection<Camera> : std::true_type {};
// @@END

// @@BEGIN 0//home/el3tek/Projects/C-C++/an-engine/scene2/components/mesh_renderer.h
#include "./scene2/components/mesh_renderer.h"
template<> struct has_reflection<MeshRenderer> : std::true_type {};
// @@END

// @@BEGIN 0//home/el3tek/Projects/C-C++/an-engine/scene2/components/panel.h
#include "./scene2/components/panel.h"
template<> struct has_reflection<Panel> : std::true_type {};
// @@END

// @@BEGIN 0//home/el3tek/Projects/C-C++/an-engine/scene2/components/script_component.h
#include "./scene2/components/script_component.h"
template<> struct has_reflection<ScriptComponent> : std::true_type {};
// @@END

// @@BEGIN 0//home/el3tek/Projects/C-C++/an-engine/scene2/components/sprite_renderer.h
#include "./scene2/components/sprite_renderer.h"
template<> struct has_reflection<SpriteRenderer> : std::true_type {};
// @@END

// @@BEGIN 0//home/el3tek/Projects/C-C++/an-engine/scene2/components/transform.h
#include "./scene2/components/transform.h"
template<> struct has_reflection<Transform> : std::true_type {};
// @@END

// @@BEGIN 0//home/el3tek/Projects/C-C++/an-engine/scene2/systems/physics_system.h
#include "./scene2/systems/physics_system.h"
template<> struct has_reflection<AABB> : std::true_type {};
#include "./scene2/systems/physics_system.h"
template<> struct has_reflection<RigidBody> : std::true_type {};
// @@END

// @@BEGIN 1//home/el3tek/Projects/C-C++/an-engine/resources/style_box.h
// StyleBox

template<>
inline TypeClass* get_class_impl<StyleBox>() {
    static TypeClass c("StyleBox", sizeof(StyleBox));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<StyleBox>(); };
    c.create_instance = []() -> void* { return new StyleBox(); };

    c.parent_type = static_cast<TypeClass*>(get_type_by_name("Resource"));

    {
        static const std::string_view _fname_bg_color = "bg_color";
        static const std::string_view _ftype_bg_color = "Color";
        Field f;
        f.name = _fname_bg_color;
        f.offset = offsetof(StyleBox, bg_color);
        f.type = get_type<Color>();
        if (f.type) f.type->set_name(_ftype_bg_color);
        else LOG_ERROR("reflect: field 'bg_color' of StyleBox has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_border_width = "border_width";
        static const std::string_view _ftype_border_width = "Vector4";
        Field f;
        f.name = _fname_border_width;
        f.offset = offsetof(StyleBox, border_width);
        f.type = get_type<Vector4>();
        if (f.type) f.type->set_name(_ftype_border_width);
        else LOG_ERROR("reflect: field 'border_width' of StyleBox has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_border_color = "border_color";
        static const std::string_view _ftype_border_color = "Color";
        Field f;
        f.name = _fname_border_color;
        f.offset = offsetof(StyleBox, border_color);
        f.type = get_type<Color>();
        if (f.type) f.type->set_name(_ftype_border_color);
        else LOG_ERROR("reflect: field 'border_color' of StyleBox has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_corner_radius = "corner_radius";
        static const std::string_view _ftype_corner_radius = "Vector4";
        Field f;
        f.name = _fname_corner_radius;
        f.offset = offsetof(StyleBox, corner_radius);
        f.type = get_type<Vector4>();
        if (f.type) f.type->set_name(_ftype_corner_radius);
        else LOG_ERROR("reflect: field 'corner_radius' of StyleBox has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_expand_margins = "expand_margins";
        static const std::string_view _ftype_expand_margins = "Vector4";
        Field f;
        f.name = _fname_expand_margins;
        f.offset = offsetof(StyleBox, expand_margins);
        f.type = get_type<Vector4>();
        if (f.type) f.type->set_name(_ftype_expand_margins);
        else LOG_ERROR("reflect: field 'expand_margins' of StyleBox has unresolved type");
        c.get_fields().push_back(f);
    }

    return &c;
}

namespace {
    struct StyleBox_AutoRegister {
        StyleBox_AutoRegister() {
            register_type<StyleBox>(get_class_impl<StyleBox>());
        }
    };
    inline StyleBox_AutoRegister StyleBox_auto_register_instance;
}


// @@END

// @@BEGIN 1//home/el3tek/Projects/C-C++/an-engine/scene2/component.h
// Component

template<>
inline TypeClass* get_class_impl<Component>() {
    static TypeClass c("Component", sizeof(Component));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<Component>(); };
    c.create_instance = []() -> void* { return new Component(); };

    {
        static const std::string_view _fname_active = "active";
        static const std::string_view _ftype_active = "bool";
        Field f;
        f.name = _fname_active;
        f.offset = offsetof(Component, active);
        f.type = get_type<bool>();
        if (f.type) f.type->set_name(_ftype_active);
        else LOG_ERROR("reflect: field 'active' of Component has unresolved type");
        c.get_fields().push_back(f);
    }

    return &c;
}

namespace {
    struct Component_AutoRegister {
        Component_AutoRegister() {
            register_type<Component>(get_class_impl<Component>());
        }
    };
    inline Component_AutoRegister Component_auto_register_instance;
}


// @@END

// @@BEGIN 1//home/el3tek/Projects/C-C++/an-engine/scene2/components/camera.h
// Camera
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
        if (f.type) f.type->set_name(_ftype_type);
        else LOG_ERROR("reflect: field 'type' of Camera has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_fov = "fov";
        static const std::string_view _ftype_fov = "float";
        Field f;
        f.name = _fname_fov;
        f.offset = offsetof(Camera, fov);
        f.type = get_type<float>();
        if (f.type) f.type->set_name(_ftype_fov);
        else LOG_ERROR("reflect: field 'fov' of Camera has unresolved type");
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
        if (f.type) f.type->set_name(_ftype_orthographic_size);
        else LOG_ERROR("reflect: field 'orthographic_size' of Camera has unresolved type");
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
        if (f.type) f.type->set_name(_ftype_near_plane);
        else LOG_ERROR("reflect: field 'near_plane' of Camera has unresolved type");
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
        if (f.type) f.type->set_name(_ftype_far_plane);
        else LOG_ERROR("reflect: field 'far_plane' of Camera has unresolved type");
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


// @@END

// @@BEGIN 1//home/el3tek/Projects/C-C++/an-engine/scene2/components/mesh_renderer.h
// MeshRenderer
#include "scene2/components/component_ops.h"

template<>
inline TypeClass* get_class_impl<MeshRenderer>() {
    static TypeClass c("MeshRenderer", sizeof(MeshRenderer));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<MeshRenderer>(); };
    c.create_instance = []() -> void* { return new MeshRenderer(); };

    c.parent_type = static_cast<TypeClass*>(get_type_by_name("Component"));

    {
        static const std::string_view _fname_mesh = "mesh";
        static const std::string_view _ftype_mesh = "Mesh *";
        Field f;
        f.name = _fname_mesh;
        f.offset = offsetof(MeshRenderer, mesh);
        f.type = get_type<Mesh *>();
        if (f.type) f.type->set_name(_ftype_mesh);
        else LOG_ERROR("reflect: field 'mesh' of MeshRenderer has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_materials = "materials";
        static const std::string_view _ftype_materials = "std::vector<Material *>";
        Field f;
        f.name = _fname_materials;
        f.offset = offsetof(MeshRenderer, materials);
        f.type = get_type<std::vector<Material *>>();
        if (f.type) f.type->set_name(_ftype_materials);
        else LOG_ERROR("reflect: field 'materials' of MeshRenderer has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_is_static = "is_static";
        static const std::string_view _ftype_is_static = "bool";
        Field f;
        f.name = _fname_is_static;
        f.offset = offsetof(MeshRenderer, is_static);
        f.type = get_type<bool>();
        if (f.type) f.type->set_name(_ftype_is_static);
        else LOG_ERROR("reflect: field 'is_static' of MeshRenderer has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string _fnname_set_material = "set_material";
        static const std::string _fnret_set_material = "void";
        RefFunction fn;
        fn.name = _fnname_set_material.c_str();
        fn.return_value.name = _fnname_set_material.c_str();
        fn.return_value.offset = 0;
        {
            static const std::string _pname_set_material_0 = "index";
            static const std::string _ptype_set_material_0 = "int";
            Field p;
            p.name   = _pname_set_material_0.c_str();
            p.offset = 0;
            p.type   = get_type<int>();
            fn.parameters.push_back(p);
        }
        {
            static const std::string _pname_set_material_1 = "mat";
            static const std::string _ptype_set_material_1 = "Material *";
            Field p;
            p.name   = _pname_set_material_1.c_str();
            p.offset = 1;
            p.type   = get_type<Material *>();
            fn.parameters.push_back(p);
        }
        fn.invoke = [](void* self, void** args, void* ret) {
            auto* obj = static_cast<MeshRenderer*>(self);
            obj->set_material(*static_cast<int*>(args[0]), *static_cast<Material **>(args[1]));
        };
        c.get_functions().push_back(fn);
    }
    {
        static const std::string _fnname_get_material = "get_material";
        static const std::string _fnret_get_material = "Material *";
        RefFunction fn;
        fn.name = _fnname_get_material.c_str();
        fn.return_value.name = _fnname_get_material.c_str();
        fn.return_value.offset = 0;
        fn.return_value.type   = get_type<Material *>();
        {
            static const std::string _pname_get_material_0 = "index";
            static const std::string _ptype_get_material_0 = "int";
            Field p;
            p.name   = _pname_get_material_0.c_str();
            p.offset = 0;
            p.type   = get_type<int>();
            fn.parameters.push_back(p);
        }
        fn.invoke = [](void* self, void** args, void* ret) {
            auto* obj = static_cast<MeshRenderer*>(self);
            *static_cast<Material **>(ret) = obj->get_material(*static_cast<int*>(args[0]));
        };
        c.get_functions().push_back(fn);
    }
    {
        static const std::string _fnname_add_material = "add_material";
        static const std::string _fnret_add_material = "void";
        RefFunction fn;
        fn.name = _fnname_add_material.c_str();
        fn.return_value.name = _fnname_add_material.c_str();
        fn.return_value.offset = 0;
        {
            static const std::string _pname_add_material_0 = "mat";
            static const std::string _ptype_add_material_0 = "Material *";
            Field p;
            p.name   = _pname_add_material_0.c_str();
            p.offset = 0;
            p.type   = get_type<Material *>();
            fn.parameters.push_back(p);
        }
        fn.invoke = [](void* self, void** args, void* ret) {
            auto* obj = static_cast<MeshRenderer*>(self);
            obj->add_material(*static_cast<Material **>(args[0]));
        };
        c.get_functions().push_back(fn);
    }

    component_ops_registry()[&c] = ComponentTypeOps{
        [](Scene& scene, Entity id) -> Component* {
            return &scene.assign_component<MeshRenderer>(id);
        },
        [](Scene& scene, Entity id) -> Component* {
            return &scene.get_component<MeshRenderer>(id);
        },
        [](Scene& scene, Entity id) -> bool {
            return scene.get_entities()[entity::index(id)].mask.test(type_id<MeshRenderer>());
        },
        [](Scene& scene, Entity id) -> void {
            scene.remove_component<MeshRenderer>(id);
        },
    };

    return &c;
}

namespace {
    struct MeshRenderer_AutoRegister {
        MeshRenderer_AutoRegister() {
            register_type<MeshRenderer>(get_class_impl<MeshRenderer>());
        }
    };
    inline MeshRenderer_AutoRegister MeshRenderer_auto_register_instance;
}


// @@END

// @@BEGIN 1//home/el3tek/Projects/C-C++/an-engine/scene2/components/panel.h
// Panel
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


// @@END

// @@BEGIN 1//home/el3tek/Projects/C-C++/an-engine/scene2/components/script_component.h
// ScriptComponent
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


// @@END

// @@BEGIN 1//home/el3tek/Projects/C-C++/an-engine/scene2/components/sprite_renderer.h
// SpriteRenderer
#include "scene2/components/component_ops.h"

template<>
inline TypeClass* get_class_impl<SpriteRenderer>() {
    static TypeClass c("SpriteRenderer", sizeof(SpriteRenderer));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<SpriteRenderer>(); };
    c.create_instance = []() -> void* { return new SpriteRenderer(); };

    c.parent_type = static_cast<TypeClass*>(get_type_by_name("Component"));

    {
        static const std::string_view _fname_sprite = "sprite";
        static const std::string_view _ftype_sprite = "Sprite *";
        Field f;
        f.name = _fname_sprite;
        f.offset = offsetof(SpriteRenderer, sprite);
        f.type = get_type<Sprite *>();
        if (f.type) f.type->set_name(_ftype_sprite);
        else LOG_ERROR("reflect: field 'sprite' of SpriteRenderer has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_color = "color";
        static const std::string_view _ftype_color = "Color";
        Field f;
        f.name = _fname_color;
        f.offset = offsetof(SpriteRenderer, color);
        f.type = get_type<Color>();
        if (f.type) f.type->set_name(_ftype_color);
        else LOG_ERROR("reflect: field 'color' of SpriteRenderer has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_flip_x = "flip_x";
        static const std::string_view _ftype_flip_x = "bool";
        Field f;
        f.name = _fname_flip_x;
        f.offset = offsetof(SpriteRenderer, flip_x);
        f.type = get_type<bool>();
        if (f.type) f.type->set_name(_ftype_flip_x);
        else LOG_ERROR("reflect: field 'flip_x' of SpriteRenderer has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_flip_y = "flip_y";
        static const std::string_view _ftype_flip_y = "bool";
        Field f;
        f.name = _fname_flip_y;
        f.offset = offsetof(SpriteRenderer, flip_y);
        f.type = get_type<bool>();
        if (f.type) f.type->set_name(_ftype_flip_y);
        else LOG_ERROR("reflect: field 'flip_y' of SpriteRenderer has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_layer = "layer";
        static const std::string_view _ftype_layer = "int";
        Field f;
        f.name = _fname_layer;
        f.offset = offsetof(SpriteRenderer, layer);
        f.type = get_type<int>();
        if (f.type) f.type->set_name(_ftype_layer);
        else LOG_ERROR("reflect: field 'layer' of SpriteRenderer has unresolved type");
        c.get_fields().push_back(f);
    }

    component_ops_registry()[&c] = ComponentTypeOps{
        [](Scene& scene, Entity id) -> Component* {
            return &scene.assign_component<SpriteRenderer>(id);
        },
        [](Scene& scene, Entity id) -> Component* {
            return &scene.get_component<SpriteRenderer>(id);
        },
        [](Scene& scene, Entity id) -> bool {
            return scene.get_entities()[entity::index(id)].mask.test(type_id<SpriteRenderer>());
        },
        [](Scene& scene, Entity id) -> void {
            scene.remove_component<SpriteRenderer>(id);
        },
    };

    return &c;
}

namespace {
    struct SpriteRenderer_AutoRegister {
        SpriteRenderer_AutoRegister() {
            register_type<SpriteRenderer>(get_class_impl<SpriteRenderer>());
        }
    };
    inline SpriteRenderer_AutoRegister SpriteRenderer_auto_register_instance;
}


// @@END

// @@BEGIN 1//home/el3tek/Projects/C-C++/an-engine/scene2/components/transform.h
// Transform
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
        if (f.type) f.type->set_name(_ftype_position);
        else LOG_ERROR("reflect: field 'position' of Transform has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_rotation = "rotation";
        static const std::string_view _ftype_rotation = "Quaternion";
        Field f;
        f.name = _fname_rotation;
        f.offset = offsetof(Transform, rotation);
        f.type = get_type<Quaternion>();
        if (f.type) f.type->set_name(_ftype_rotation);
        else LOG_ERROR("reflect: field 'rotation' of Transform has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_scale = "scale";
        static const std::string_view _ftype_scale = "Vector3";
        Field f;
        f.name = _fname_scale;
        f.offset = offsetof(Transform, scale);
        f.type = get_type<Vector3>();
        if (f.type) f.type->set_name(_ftype_scale);
        else LOG_ERROR("reflect: field 'scale' of Transform has unresolved type");
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


// @@END

// @@BEGIN 1//home/el3tek/Projects/C-C++/an-engine/scene2/systems/physics_system.h
// AABB
#include "scene2/components/component_ops.h"

template<>
inline TypeClass* get_class_impl<AABB>() {
    static TypeClass c("AABB", sizeof(AABB));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<AABB>(); };
    c.create_instance = []() -> void* { return new AABB(); };

    c.parent_type = static_cast<TypeClass*>(get_type_by_name("Component"));


    component_ops_registry()[&c] = ComponentTypeOps{
        [](Scene& scene, Entity id) -> Component* {
            return &scene.assign_component<AABB>(id);
        },
        [](Scene& scene, Entity id) -> Component* {
            return &scene.get_component<AABB>(id);
        },
        [](Scene& scene, Entity id) -> bool {
            return scene.get_entities()[entity::index(id)].mask.test(type_id<AABB>());
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
            register_type<AABB>(get_class_impl<AABB>());
        }
    };
    inline AABB_AutoRegister AABB_auto_register_instance;
}


// RigidBody
#include "scene2/components/component_ops.h"

template<>
inline TypeClass* get_class_impl<RigidBody>() {
    static TypeClass c("RigidBody", sizeof(RigidBody));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<RigidBody>(); };
    c.create_instance = []() -> void* { return new RigidBody(); };

    c.parent_type = static_cast<TypeClass*>(get_type_by_name("Component"));

    {
        static const std::string_view _fname_velocity = "velocity";
        static const std::string_view _ftype_velocity = "Vector3";
        Field f;
        f.name = _fname_velocity;
        f.offset = offsetof(RigidBody, velocity);
        f.type = get_type<Vector3>();
        if (f.type) f.type->set_name(_ftype_velocity);
        else LOG_ERROR("reflect: field 'velocity' of RigidBody has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_mass = "mass";
        static const std::string_view _ftype_mass = "float";
        Field f;
        f.name = _fname_mass;
        f.offset = offsetof(RigidBody, mass);
        f.type = get_type<float>();
        if (f.type) f.type->set_name(_ftype_mass);
        else LOG_ERROR("reflect: field 'mass' of RigidBody has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_gravity_scale = "gravity_scale";
        static const std::string_view _ftype_gravity_scale = "float";
        Field f;
        f.name = _fname_gravity_scale;
        f.offset = offsetof(RigidBody, gravity_scale);
        f.type = get_type<float>();
        if (f.type) f.type->set_name(_ftype_gravity_scale);
        else LOG_ERROR("reflect: field 'gravity_scale' of RigidBody has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_restitution = "restitution";
        static const std::string_view _ftype_restitution = "float";
        Field f;
        f.name = _fname_restitution;
        f.offset = offsetof(RigidBody, restitution);
        f.type = get_type<float>();
        if (f.type) f.type->set_name(_ftype_restitution);
        else LOG_ERROR("reflect: field 'restitution' of RigidBody has unresolved type");
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_is_static = "is_static";
        static const std::string_view _ftype_is_static = "bool";
        Field f;
        f.name = _fname_is_static;
        f.offset = offsetof(RigidBody, is_static);
        f.type = get_type<bool>();
        if (f.type) f.type->set_name(_ftype_is_static);
        else LOG_ERROR("reflect: field 'is_static' of RigidBody has unresolved type");
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
            return scene.get_entities()[entity::index(id)].mask.test(type_id<RigidBody>());
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
            register_type<RigidBody>(get_class_impl<RigidBody>());
        }
    };
    inline RigidBody_AutoRegister RigidBody_auto_register_instance;
}


// @@END

