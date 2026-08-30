// Generated reflection for: SpriteRenderer
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./scene2/components/sprite_renderer.h"
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
        f.type->set_name(_ftype_sprite);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_color = "color";
        static const std::string_view _ftype_color = "Color";
        Field f;
        f.name = _fname_color;
        f.offset = offsetof(SpriteRenderer, color);
        f.type = get_type<Color>();
        f.type->set_name(_ftype_color);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_flip_x = "flip_x";
        static const std::string_view _ftype_flip_x = "bool";
        Field f;
        f.name = _fname_flip_x;
        f.offset = offsetof(SpriteRenderer, flip_x);
        f.type = get_type<bool>();
        f.type->set_name(_ftype_flip_x);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_flip_y = "flip_y";
        static const std::string_view _ftype_flip_y = "bool";
        Field f;
        f.name = _fname_flip_y;
        f.offset = offsetof(SpriteRenderer, flip_y);
        f.type = get_type<bool>();
        f.type->set_name(_ftype_flip_y);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_layer = "layer";
        static const std::string_view _ftype_layer = "int";
        Field f;
        f.name = _fname_layer;
        f.offset = offsetof(SpriteRenderer, layer);
        f.type = get_type<int>();
        f.type->set_name(_ftype_layer);
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


