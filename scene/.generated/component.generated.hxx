// Generated reflection for: Component
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "scene/component.h"

template<>
inline TypeClass* get_class_impl<Component>() {
    static TypeClass c("Component", sizeof(Component));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<Component>(); };
    c.create_instance = []() -> void* { return new Component(); };

    {
        static const std::string _fnname_get_type_name = "get_type_name";
        static const std::string _fnret_get_type_name = "std::string";
        RefFunction fn;
        fn.name = _fnname_get_type_name.c_str();
        fn.return_value.name = _fnname_get_type_name.c_str();
        fn.return_value.offset = 0;
        fn.return_value.type   = get_type<std::string>();
        fn.invoke = [](void* self, void** args, void* ret) {
            auto* obj = static_cast<Component*>(self);
            *static_cast<std::string*>(ret) = obj->get_type_name();
        };
        c.get_functions().push_back(fn);
    }

    return &c;
}

namespace {
    struct Component_AutoRegister {
        Component_AutoRegister() {
            CLASS_REGISTRY["Component"] = get_class_impl<Component>();
            register_type<Component>("Component")
;        }
    };
    inline Component_AutoRegister Component_auto_register_instance;
}

