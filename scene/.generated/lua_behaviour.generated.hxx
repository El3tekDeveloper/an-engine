// Generated reflection for: LuaBehaviour
// DO NOT EDIT
#pragma once
#include "tools/reflector/type_registry.h"
#include "scene/lua_behaviour.h"

template<>
inline TypeClass* get_class_impl<LuaBehaviour>() {
    static TypeClass c("LuaBehaviour", sizeof(LuaBehaviour));
    static bool initialized = false;
    if (initialized) return &c;
    initialized = true;

    c.get_impl = []() -> TypeClass* { return get_class_impl<LuaBehaviour>(); };
    c.create_instance = []() -> void* { return new LuaBehaviour(); };

    c.parent_type = get_class_by_name("Behaviour");

    {
        static const std::string_view _fname_script_path = "script_path";
        static const std::string_view _ftype_script_path = "std::string";
        Field f;
        f.name = _fname_script_path;
        f.offset = offsetof(LuaBehaviour, script_path);
        f.type = get_type<std::string>();
        f.type->set_name(_ftype_script_path);
        c.get_fields().push_back(f);
    }

    return &c;
}

namespace {
    struct LuaBehaviour_AutoRegister {
        LuaBehaviour_AutoRegister() {
            CLASS_REGISTRY["LuaBehaviour"] = get_class_impl<LuaBehaviour>();
            register_type<LuaBehaviour>("LuaBehaviour")
;        }
    };
    inline LuaBehaviour_AutoRegister LuaBehaviour_auto_register_instance;
}

