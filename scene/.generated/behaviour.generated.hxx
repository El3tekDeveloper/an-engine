// Generated reflection for: Behaviour
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "scene/behaviour.h"

template<>
inline TypeClass* get_class_impl<Behaviour>() {
    static TypeClass c("Behaviour", sizeof(Behaviour));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<Behaviour>(); };
    c.create_instance = []() -> void* { return new Behaviour(); };

    c.parent_type = get_class_by_name("Component");


    return &c;
}

namespace {
    struct Behaviour_AutoRegister {
        Behaviour_AutoRegister() {
            CLASS_REGISTRY["Behaviour"] = get_class_impl<Behaviour>();
            register_type<Behaviour>("Behaviour")
;        }
    };
    inline Behaviour_AutoRegister Behaviour_auto_register_instance;
}

