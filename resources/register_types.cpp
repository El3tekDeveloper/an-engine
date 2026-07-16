#include "render/renderer.h"
#include "resources/mesh.h"
#include "tools/reflector/type_registry.h"
#include <format>
#include <string>

const bool resources_registered = [] {
    register_type<Resource>(
        "Resource",
        [](const void* p) -> std::string {
            const auto& r = *static_cast<const Resource*>(p);
            return std::format("Resource({})", r.get_uuid());
        },
        TypeKind::Class
    );

    register_type<Material>(
        "Material",
        [](const void* p) -> std::string {
            const auto& m = *static_cast<const Material*>(p);
            return std::format("Material({})", m.get_uuid());
        },
        TypeKind::Class
    );

    register_type<Mesh>(
        "Mesh",
        [](const void* p) -> std::string {
            const auto& m = *static_cast<const Mesh*>(p);
            return std::format("Mesh({})", m.name);
        },
        TypeKind::Class
    );
    
    register_type<Sprite>(
        "Sprite",
        [](const void* p) -> std::string {
            const auto& s = *static_cast<const Sprite*>(p);
            return std::format("Sprite({})", s.get_uuid());
        },
        TypeKind::Class
    );

    register_type<ViewPort>(
        "ViewPort",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const ViewPort*>(p);
            return std::format("{},{}", v.width, v.height);
        },
        TypeKind::Class
    );

    return true;
}();
