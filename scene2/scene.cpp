#include "scene.h"
#include "core/io/file_access.h"
#include "ecs.h"
#include "registry.h"
#include "game_object.h"
#include <memory>
#include <sstream>
#include <vector>

void Scene::initialize() {
    registry = new ecs::Registry(*this);

    for (auto& system : systems) {
        system->awake(*this);
    }
    initialized = true;
}

void Scene::update() {
    for (auto& system : systems) {
        system->update_internal(*this);
    }
}

void Scene::draw(RenderData& rd) {
    for (auto& system : systems) {
        system->draw(*this, rd);
    }
}

void Scene::add_system(std::unique_ptr<System> system) {
    systems.push_back(std::move(system));
}

void Scene::save(const std::string& path) {
    std::ostringstream out;
    out << "[scene uid=\"" << std::to_string(make_uuid(path)) << "\"]\n\n";

    for (auto& [id, mask] : entities) {
        if (mask.none())
            continue;

        out << std::format("[entity id=\"{}\"]\n", id);

        for (ComponentType type = 0; type < MAX_COMPONENETS; ++type) {
            if (!mask.test(type))
                continue;

            void* raw = component_pools[type]->get(entity::index(id));
            Component* comp = static_cast<Component*>(raw);

            out << std::format("- [component type_name=\"{}\"]\n", comp->get_type_name());

            TypeClass* tc = get_class_by_name(comp->get_type_name());
            if (tc) {
                for (Field& field : tc->get_fields()) {
                    if (!field.is_valid()) continue;

                    const void* field_ptr = reinterpret_cast<const std::byte*>(
                        static_cast<const void*>(comp)
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

Entity Scene::create_entity() {
    if (!free_entities.empty()) {
        uint32_t index = free_entities.back();
        free_entities.pop_back();

        Entity new_id = entity::make(index, entity::version(entities[index].first));
        entities[index].first = new_id;
        return entities[index].first;
    }
    
    entities.emplace_back(entity::make(entity::next_index(entities.size()), 0), ComponentMask());
    return entities.back().first;
}

void Scene::destroy_entity(Entity id) {
    Entity new_id = entity::make(entity::index(-1), entity::version(id) + 1);
    entities[entity::index(id)].first = new_id;
    entities[entity::index(id)].second.reset();
    free_entities.push_back(entity::index(id));
}

GameObject Scene::create_game_object() {
    Entity new_entity_id = create_entity();
    assign_component<Transform>(new_entity_id);
    return GameObject{ new_entity_id, this };
}

void Scene::destroy_game_object(GameObject game_object) {
    destroy_entity(game_object.id);
}
