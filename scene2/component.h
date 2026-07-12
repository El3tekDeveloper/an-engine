#pragma once
#include "core/utils/demangle.h"
#include "scene2/scene.h"
#include "tools/reflector/type_registry.h"
#include <string>

struct [[export]] Component {
    virtual ~Component() = default;
    [[export]] bool active = true;
    
    virtual inline std::string get_type_name() const {
        return type_name(*this);
    }
};


struct ComponentTypeOps {
    Component* (*add)(Scene& scene, Entity id) = nullptr;
    Component* (*get)(Scene& scene, Entity id) = nullptr;
    bool (*has)(Scene& scene, Entity id) = nullptr;
    void (*remove)(Scene& scene, Entity id) = nullptr;
};

inline std::unordered_map<TypeClass*, ComponentTypeOps>& component_ops_registry() {
    static std::unordered_map<TypeClass*, ComponentTypeOps> registry;
    return registry;
}

inline const ComponentTypeOps* find_component_ops(TypeClass* tc) {
    if (!tc) return nullptr;
    auto& registry = component_ops_registry();
    auto it = registry.find(tc);
    return it != registry.end() ? &it->second : nullptr;
}

#if __REFLECT_GENERATED__
#include ".generated/component.generated.hxx"
#endif
