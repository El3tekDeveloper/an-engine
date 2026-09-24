#pragma once
#include "scene2/component.h"
#include "scene2/scene.h"

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
