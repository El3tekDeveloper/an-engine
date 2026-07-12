#pragma once
#include "tools/reflector/generator_plugin.h"
#include "tools/reflector/generator_plugin.h"
#include <clang/AST/DeclCXX.h>

namespace component_ops_detail {

inline bool derives_from_component(const clang::CXXRecordDecl* rec) {
    if (!rec)
        return false;

    for (const auto& base : rec->bases()) {
        const clang::CXXRecordDecl* base_decl = base.getType()->getAsCXXRecordDecl();
        if (!base_decl)
            continue;

        if (base_decl->getNameAsString() == "Component")
            return true;

        if (derives_from_component(base_decl))
            return true;
    }
    return false;
}

} // namespace component_ops_detail

class ComponentOpsPlugin : public GeneratorPlugin {
public:
    void on_class_end(const clang::CXXRecordDecl* rec, const GenContext& gc) override {
        if (gc.scope_var.empty())
            return;

        if (!component_ops_detail::derives_from_component(rec))
            return;

        *gc.os << "\n";
        *gc.os << "    component_ops_registry()[&" << gc.scope_var << "] = ComponentTypeOps{\n";
        *gc.os << "        [](Scene& scene, Entity id) -> Component* {\n";
        *gc.os << "            return &scene.assign_component<" << gc.type << ">(id);\n";
        *gc.os << "        },\n";
        *gc.os << "        [](Scene& scene, Entity id) -> Component* {\n";
        *gc.os << "            return &scene.get_component<" << gc.type << ">(id);\n";
        *gc.os << "        },\n";
        *gc.os << "        [](Scene& scene, Entity id) -> bool {\n";
        *gc.os << "            return scene.get_entities()[entity::index(id)].second.test(type_id<"<< gc.type <<">());\n";
        *gc.os << "        },\n";
        *gc.os << "        [](Scene& scene, Entity id) -> void {\n";
        *gc.os << "            scene.remove_component<" << gc.type << ">(id);\n";
        *gc.os << "        },\n";
        *gc.os << "    };\n";
    }
};

REGISTER_GENERATOR(ComponentOpsPlugin)
