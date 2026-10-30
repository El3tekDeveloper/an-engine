#pragma once
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <unordered_map>
#include "component.h"
#include "core/io/file_access.h"
#include "core/utils/uid.h"
#include "scene.h"
#include "tools/reflector/runtime/type_registry.h"

using SceneValues = std::unordered_map<std::string, std::string>;

template<typename Fn>
void each_scene_component(Scene& scene, Fn&& fn) {
    for (const EntityRecord& entity : scene.get_entities()) {
        for (ComponentType type = 0; type < MAX_COMPONENETS; ++type) {
            if (!entity.mask.test(type)) continue;

            Component* component = scene.get_component(entity.id, type);
            fn(entity.id, component, component->get_type_name());
        }
    }
}

template<typename Fn>
void each_component_field(Component* component, const std::string& type_name, Fn&& fn) {
    auto* type_class = static_cast<TypeClass*>(get_type_by_name(type_name));
    if (!type_class) return;

    for (Field& field : type_class->get_fields()) {
        if (!field.is_valid()) continue;

        void* value = reinterpret_cast<std::byte*>(component) + field.offset;
        fn(field, value);
    }
}

template<typename Fn>
void each_scene_field(Scene& scene, Fn&& fn) {
    each_scene_component(scene, [&](Entity id, Component* component, const std::string& type_name) {
        each_component_field(component, type_name, [&](Field& field, void* value) {
            if (field.type->get_name().ends_with('*')) return;
            fn(std::format("{}/{}/{}", id, type_name, field.name), field, value);
        });
    });
}

inline SceneValues capture_scene_values(Scene& scene) {
    SceneValues values;
    each_scene_field(scene, [&](std::string key, Field& field, void* value) {
        values.emplace(std::move(key), field.type->to_string_value(value));
    });
    return values;
}

inline void restore_scene_values(Scene& scene, const SceneValues& previous_start, const SceneValues& live) {
    each_scene_field(scene, [&](const std::string& key, Field& field, void* value) {
        auto start = previous_start.find(key);
        auto running = live.find(key);
        if (start == previous_start.end() || running == live.end()) return;

        if (field.type->to_string_value(value) == start->second)
            field.type->from_string_value(running->second, value);
    });
}

inline void save_scene(Scene& scene, const std::string& path, const std::string& name = "") {
    std::ostringstream out;
    out << std::format("[scene name=\"{}\" uid=\"{}\"]\n", name, make_uid(path));

    Entity current = entity::INVALID;
    each_scene_component(scene, [&](Entity id, Component* component, const std::string& type_name) {
        if (id != current) out << std::format("\n[entity id=\"{}\"]\n", id);
        current = id;

        out << std::format("- [component type_name=\"{}\"]\n", type_name);
        each_component_field(component, type_name, [&](Field& field, void* value) {
            out << std::format("  - {} = {}\n", field.name, field.type->to_string_value(value));
        });
    });
    out << "\n";

    if (!IO::write_file(path, out.str())) LOG_ERROR("Failed to save scene '{}'", path);
}

inline std::string read_scene_name(const std::string& path) {
    std::istringstream in(IO::read_file(path));
    std::string line;
    std::getline(in, line);

    const std::string key = "name=\"";
    const size_t start = line.find(key);
    if (start == std::string::npos) return "";

    const size_t begin = start + key.size();
    return line.substr(begin, line.find('"', begin) - begin);
}

inline SceneValues read_scene_values(const std::string& path) {
    const auto quoted = [](const std::string& line) {
        const size_t first = line.find('"');
        return line.substr(first + 1, line.rfind('"') - first - 1);
    };

    SceneValues values;
    std::istringstream in(IO::read_file(path));
    std::string line, entity, component;

    while (std::getline(in, line)) {
        if (line.starts_with("[entity ")) entity = quoted(line);
        else if (line.starts_with("- [component ")) component = quoted(line);
        else if (line.starts_with("  - ")) {
            const size_t equal = line.find(" = ");
            if (equal == std::string::npos) continue;

            const std::string field = line.substr(4, equal - 4);
            values.emplace(std::format("{}/{}/{}", entity, component, field), line.substr(equal + 3));
        }
    }
    return values;
}

inline void apply_scene_values(Scene& scene, const SceneValues& values) {
    each_scene_field(scene, [&](const std::string& key, Field& field, void* value) {
        auto saved = values.find(key);
        if (saved != values.end()) field.type->from_string_value(saved->second, value);
    });
}
