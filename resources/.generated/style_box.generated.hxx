// Generated reflection for: StyleBox
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./resources/style_box.h"

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
        f.type->set_name(_ftype_bg_color);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_border_width = "border_width";
        static const std::string_view _ftype_border_width = "Vector4";
        Field f;
        f.name = _fname_border_width;
        f.offset = offsetof(StyleBox, border_width);
        f.type = get_type<Vector4>();
        f.type->set_name(_ftype_border_width);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_border_color = "border_color";
        static const std::string_view _ftype_border_color = "Color";
        Field f;
        f.name = _fname_border_color;
        f.offset = offsetof(StyleBox, border_color);
        f.type = get_type<Color>();
        f.type->set_name(_ftype_border_color);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_corner_radius = "corner_radius";
        static const std::string_view _ftype_corner_radius = "Vector4";
        Field f;
        f.name = _fname_corner_radius;
        f.offset = offsetof(StyleBox, corner_radius);
        f.type = get_type<Vector4>();
        f.type->set_name(_ftype_corner_radius);
        c.get_fields().push_back(f);
    }
    {
        static const std::string_view _fname_expand_margins = "expand_margins";
        static const std::string_view _ftype_expand_margins = "Vector4";
        Field f;
        f.name = _fname_expand_margins;
        f.offset = offsetof(StyleBox, expand_margins);
        f.type = get_type<Vector4>();
        f.type->set_name(_ftype_expand_margins);
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


