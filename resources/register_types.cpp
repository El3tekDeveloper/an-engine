#include "render/viewport.h"
#include "resources/mesh.h"
#include "resources/sprite.h"
#include "tools/reflector/runtime/type_registry.h"
#include <format>
#include <string>

const bool resources_registered = [] {
    register_type<Resource>(
        "Resource",
        [](const void* p) -> std::string {
            const auto& r = *static_cast<const Resource*>(p);
            return std::format("[{}]", r.get_uid());
        },
        nullptr,
        TypeKind::Class
    );

    register_type<Material>(
        "Material",
        [](const void* p) -> std::string {
            const auto& m = *static_cast<const Material*>(p);
            return std::format("[{}]", m.get_uid());
        },
        nullptr,
        TypeKind::Class
    );

    register_type<Mesh>(
        "Mesh",
        [](const void* p) -> std::string {
            const auto& m = *static_cast<const Mesh*>(p);
            return std::format("[{}]", m.name);
        },
        nullptr,
        TypeKind::Class
    );
    
    register_type<Sprite>(
        "Sprite",
        [](const void* p) -> std::string {
            const auto& s = *static_cast<const Sprite*>(p);
            return std::format("[{}]", s.get_uid());
        },
        nullptr,
        TypeKind::Class
    );

    register_type<ViewPort>(
        "ViewPort",
        [](const void* p) -> std::string {
            const auto& v = *static_cast<const ViewPort*>(p);
            return std::format("[{},{}]", v.width, v.height);
        },
        nullptr,
        TypeKind::Class
    );

    return true;
}();
