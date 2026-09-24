#pragma once
#include "scene.h"
#include "system.h"

namespace ecs {

template<typename... ComponentType>
struct View {
    View(Scene& scene) : m_scene(&scene) {
        if (sizeof...(ComponentType) == 0) {
            all = true;
        } else {
            ecs::ComponentType component_ids[] = { type_id<ComponentType>()... };
            for (auto id : component_ids)
                component_mask.set(id);
        }
    }

    struct Iterator {
        Iterator(Scene* scene, uint32_t index, ComponentMask mask, bool all)
            : scene(scene), index(index), mask(mask), all(all) {}

        bool valid_index() {
            return entity::is_valid(scene->get_entities()[index].id)
                && (all || mask == (mask & scene->get_entities()[index].mask));
        }

        Entity operator*() const {
            return scene->get_entities()[index].id;
        }

        bool operator==(const Iterator& other) const {
            return index == other.index || index == scene->get_entities().size();
        }
        bool operator!=(const Iterator& other) const {
            return index != other.index && index != scene->get_entities().size();
        }

        Iterator& operator++() {
            do {
                index++;
            } while (index < scene->get_entities().size() && !valid_index());
            return *this;
        }

        uint32_t index;
        Scene* scene;
        ComponentMask mask;
        bool all = false;
    };

    const Iterator begin() const {
        int first_index = 0;
        while (first_index < m_scene->get_entities().size()
                && (component_mask != (component_mask & m_scene->get_entities()[first_index].mask)
                || !entity::is_valid(m_scene->get_entities()[first_index].id))) {
            first_index++;
        }
        return Iterator(m_scene, first_index, component_mask, all);
    }

    const Iterator end() const {
        return Iterator(m_scene,
            entity::next_index(m_scene->get_entities().size()),
            component_mask, all);
    }

    Scene* m_scene;
    ComponentMask component_mask;
    bool all = false;
};

class Registry {
public:
    Registry() = default;
    constexpr Registry(Scene& scene) : m_scene(&scene) {}

    ~Registry() = default;

    template<typename... ComponentType>  
    View<ComponentType...> view() { 
        return View<ComponentType...>(*m_scene); 
    }
    
    bool valid(Entity e) const {
        uint32_t idx = ecs::entity::index(e);
        return idx < m_scene->get_entities().size()
            && m_scene->get_entities()[idx].id == e
            && ecs::entity::is_valid(m_scene->get_entities()[idx].id);
    }

    template<typename... ComponentType>
    bool has(Entity e) const {
        if (!valid(e)) return false;

        ComponentMask mask{};
        ecs::ComponentType component_ids[] = { type_id<ComponentType>()... };
        for (auto id : component_ids) mask.set(id);

        const ComponentMask& entity_mask = m_scene->get_entities()[ecs::entity::index(e)].mask;
        return mask == (mask & entity_mask);
    }

    template<typename... ComponentType>
    bool has_any(Entity e) const {
        if (!valid(e)) return false;

        ComponentMask mask{};
        ecs::ComponentType component_ids[] = { type_id<ComponentType>()... };
        for (auto id : component_ids) mask.set(id);

        const ComponentMask& entity_mask = m_scene->get_entities()[ecs::entity::index(e)].mask;
        return (mask & entity_mask).any();
    }

    ComponentMask mask_of(Entity e) const {
        return valid(e) ? m_scene->get_entities()[ecs::entity::index(e)].mask : ComponentMask{};
    }

    template<typename T, typename... Args>
    inline T& add(Entity e, Args&&... args) {
        return m_scene->assign_component<T>(e, std::forward<Args>(args)...);
    }

    template<typename T>
    inline T& get(Entity e) {
        return m_scene->get_component<T>(e);
    }

    template<typename T>
    inline void remove(Entity e) {
        m_scene->remove_component<T>(e);
    }

    inline Entity create() {
        return m_scene->create_entity();
    }

    inline void destroy(Entity e) {
        m_scene->destroy(e);
    }

    size_t alive_count() const {
        size_t count = 0;
        for (auto& entity : m_scene->get_entities())
            if (ecs::entity::is_valid(entity.id)) ++count;
        return count;
    }

    template<typename Fn>
    inline void each(Fn&& fn) {
        for (auto& entity : m_scene->get_entities())
            if (ecs::entity::is_valid(entity.id)) fn(entity);
    }

private:
    Scene* m_scene;

    friend class Scene;
};

} // namespace ecs
