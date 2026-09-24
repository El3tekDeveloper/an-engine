#include "scene.h"
#include "core/debug/logger.h"
#include "core/io/file_access.h"
#include "core/utils/uid.h"
#include "game_object.h"
#include "render/renderer.h"
#include "scene/camera_component.h"
#include "scene/component.h"
#include "tools/reflector/runtime/type_registry.h"
#include <algorithm>
#include <format>
#include <memory>
#include <sstream>
#include <string>

GameObject& Scene::create_game_object(std::string name) {
    if (name.empty()) 
        name = "GameObject " + std::to_string(count());
    
    auto go = std::make_unique<GameObject>(name);
    GameObject& ref = *go;
    ref.uid = make_uid();
    game_objects.emplace_back(std::move(go));
    return ref;
}

GameObject& Scene::create_camera(std::string name) {
    if (name.empty())
        name = "Camera";

    GameObject& camera = create_game_object(name);
    camera.add_component<Camera>();
    camera.tag = Tag::Camera;

    return camera;
}

Camera* Scene::resolve_camera() {
    for (auto& go : game_objects) {
        if (!go->is_active() || go->tag != Tag::Camera) continue;
        Camera* cam = go->try_get_component<Camera>();
        if (cam) return cam;
    }
    return nullptr;
}

void Scene::initialize() {
    for_each_active_behaviour([](Behaviour& mb) {
        mb.awake();
    });
}

void Scene::update() {
    for_each_active_behaviour([](Behaviour& mb) {
        mb.update_internal();
    });
}

void Scene::draw(RenderData& rd) {
    Camera* cam = resolve_camera();
    if (!cam) {
        LOG_ERROR("No camera found");
        return;
    }
    rd.camera = cam->get_camera_data();

    for_each_active_behaviour([&](Behaviour& mb) {
        mb.draw(rd);
    });
}

void Scene::save(std::string path) {
    std::ostringstream out;
    out << "[scene uid=\"" << std::to_string(make_uid(path)) << "\"]\n\n";

    for (const auto& go : game_objects) {
        out << std::format("[game_object name=\"{}\" uid=\"{}\"]\n", go->get_name(), go->uid);

        for (auto& comp : go->components) {
            out << std::format("- [component type_name=\"{}\"]\n", comp->get_type_name());
            
            TypeClass* tc = get_class_by_name(comp->get_type_name());
            if (tc) {
                for (Field& field : tc->get_fields()) {
                    if (!field.is_valid()) continue;

                    const void* field_ptr = reinterpret_cast<const std::byte*>(
                        static_cast<const void*>(comp.get())
                    ) + field.offset;
                    std::string value = field.type->to_string_value(field_ptr);

                    out << std::format("  - {} = {}\n", field.name, value);
                }
            }
        }
        out << "\n";
    }
    
    IO::write_file(path, out.str());
}

std::string get_attribute(const std::string& line,
                          const std::string& key)
{
    std::string pattern = key + "=\"";

    auto begin = line.find(pattern);
    if (begin == std::string::npos)
        return "";

    begin += pattern.size();

    auto end = line.find('"', begin);

    return line.substr(begin, end - begin);
}

void Scene::print_tree() const {
#if ENGINE_DEBUG_ENABLED
    LOG_INFO("=== Scene Tree ({} objects) ===", game_objects.size());

    for (std::size_t i = 0; i < game_objects.size(); ++i) {
        const auto& go = game_objects[i];
        const bool is_last_go = (i == game_objects.size() - 1);
        const bool active     = go->is_active();

        LOG_INFO("{} [{}] {} (uid=\"{}\")",
            is_last_go ? "└──" : "├──",
            active ? "✓" : "✗",
            go->get_name(), go->uid);

        std::vector<std::string> entries;

        for (const auto& comp : go->components)
            if (comp) entries.push_back(comp->get_type_name());

        const std::string prefix = is_last_go ? "    " : "│   ";
        for (std::size_t j = 0; j < entries.size(); ++j) {
            const bool is_last = (j == entries.size() - 1);
            LOG_INFO("{}    {}─ {}", prefix, is_last ? "└" : "├", entries[j]);
        }
    }

    LOG_INFO("==============================");
#endif
}

GameObject* Scene::find(const std::string& name) {
    for (auto& go : game_objects) {
        if (go->is_active() && go->get_name() == name)
            return go.get();
    }
    return nullptr;
}

void Scene::destroy(GameObject* go) {
    game_objects.erase(
        std::remove_if(game_objects.begin(), game_objects.end(),
            [go](const std::unique_ptr<GameObject>& p) {
                return p.get() == go;
            }),
        game_objects.end()
    );
}
