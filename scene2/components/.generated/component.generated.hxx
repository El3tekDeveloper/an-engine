// Generated reflection for: Component
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "./scene2/components/component.h"

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
        f.type->set_name(_ftype_active);
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


