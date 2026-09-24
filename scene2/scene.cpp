#include "scene.h"
#include "core/io/file_access.h"
#include "ecs.h"
#include "registry.h"
#include "game_object.h"
#include "tools/reflector/runtime/type_registry.h"
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

void Scene::draw(ViewPort& viewport, RenderData& rd) {
    for (auto& system : systems) {
        system->draw(*this, viewport, rd);
    }
}

void Scene::add_system(std::unique_ptr<System> system) {
    systems.push_back(std::move(system));
}

void Scene::save(const std::string& path) {
    std::ostringstream out;
    out << "[scene uid=\"" << std::to_string(make_uid(path)) << "\"]\n\n";

    for (auto&  entity: entities) {
        if (entity.mask.none())
            continue;

        out << std::format("[entity id=\"{}\"]\n", entity.id);

        for (ComponentType type = 0; type < MAX_COMPONENETS; ++type) {
            if (!entity.mask.test(type))
                continue;

            void* raw = component_pools[type]->get(entity::index(entity.id));
            Component* comp = static_cast<Component*>(raw);

            out << std::format("- [component type_name=\"{}\"]\n", comp->get_type_name());

            TypeClass* tc = static_cast<TypeClass*>(get_type_by_name(comp->get_type_name()));
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

        Entity new_id = entity::make(index, entity::version(entities[index].id));
        entities[index].id = new_id;
        return entities[index].id;
    }
    
    entities.emplace_back(entity::make(entity::next_index(entities.size()), 0), ComponentMask());
    return entities.back().id;
}

void Scene::destroy(Entity id) {
    Entity new_id = entity::make(entity::index(-1), entity::version(id) + 1);
    entities[entity::index(id)].id = new_id;
    entities[entity::index(id)].mask.reset();
    free_entities.push_back(entity::index(id));
}

GameObject Scene::create_game_object() {
    Entity new_entity_id = create_entity();
    assign_component<Transform>(new_entity_id);
    return GameObject{ new_entity_id, this };
}

void Scene::destroy(GameObject game_object) {
    destroy(game_object.id);
}

void Scene::set_parent(Entity child, Entity parent) {
    if (!entity::is_valid(child))
        return;
 
    uint32_t child_index = entity::index(child);
    Entity old_parent = entities[child_index].parent;
 
    if (entity::is_valid(old_parent)) {
        auto& siblings = entities[entity::index(old_parent)].children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), child), siblings.end());
    }
 
    entities[child_index].parent = entity::INVALID;
 
    if (!entity::is_valid(parent))
        return; // set_parent(child, INVALID) == remove_parent(child)
 
    entities[child_index].parent = parent;
    entities[entity::index(parent)].children.push_back(child);
}

void Scene::remove_parent(Entity child) {
    set_parent(child, entity::INVALID);
}
 
Entity Scene::get_parent(Entity child) const {
    return entities[entity::index(child)].parent;
}
 
const std::vector<Entity>& Scene::get_children(Entity parent) const {
    return entities[entity::index(parent)].children;
}
