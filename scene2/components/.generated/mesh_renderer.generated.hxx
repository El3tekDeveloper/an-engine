// Generated reflection for: MeshRenderer
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./scene2/components/mesh_renderer.h"
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
        f.type->set_name(_ftype_mesh);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_materials = "materials";
        static const std::string_view _ftype_materials = "std::vector<Material *>";
        Field f;
        f.name = _fname_materials;
        f.offset = offsetof(MeshRenderer, materials);
        f.type = get_type<std::vector<Material *>>();
        f.type->set_name(_ftype_materials);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_is_static = "is_static";
        static const std::string_view _ftype_is_static = "bool";
        Field f;
        f.name = _fname_is_static;
        f.offset = offsetof(MeshRenderer, is_static);
        f.type = get_type<bool>();
        f.type->set_name(_ftype_is_static);
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


