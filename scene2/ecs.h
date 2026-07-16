#pragma once
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <memory_resource>

namespace ecs {
    typedef uint64_t Entity;
    typedef uint8_t ComponentType;

    constexpr Entity MAX_ENTITIES = 100000;
    constexpr ComponentType MAX_COMPONENETS = 64;
    
    namespace entity {
        constexpr uint64_t ENTITY_SHIFT = 32;
        constexpr uint64_t VERSION_MASK = 0xFFFFFFFFull;
        
        constexpr Entity make(uint32_t index, uint32_t version) {
            return (static_cast<Entity>(index) << ENTITY_SHIFT) |
                   static_cast<Entity>(version);
        }

        constexpr uint32_t index(Entity entity) {
            return static_cast<uint32_t>(entity >> ENTITY_SHIFT);
        }

        constexpr uint32_t next_index(size_t count) {
            return static_cast<uint32_t>(count);
        }

        constexpr uint32_t version(Entity entity) {
            return static_cast<uint32_t>(entity & VERSION_MASK);
        }

        constexpr bool is_valid(Entity entity) {
            return index(entity) != UINT32_MAX;
        }

        constexpr Entity INVALID = make(UINT32_MAX, 0);
    }
    
    struct ComponentPool {
        ComponentPool(size_t element_size)
            : m_element_size(element_size),
              m_resource(element_size * MAX_ENTITIES),
              m_data(static_cast<std::byte*>(
                  m_resource.allocate(element_size * MAX_ENTITIES, alignof(std::max_align_t))
              ))
        {}

        ~ComponentPool() = default;

        ComponentPool(const ComponentPool&) = delete;
        ComponentPool& operator=(const ComponentPool&) = delete;

        inline void* get(size_t index) {
            return m_data + index * m_element_size;
        }

        size_t m_element_size;
        std::pmr::monotonic_buffer_resource m_resource;
        std::byte* m_data = nullptr;
    };
    
    inline ComponentType get_unique_id() {
        static ComponentType last = 0;
        return last++;
    }

    template<typename T>
    ComponentType type_id() {
        static ComponentType id = get_unique_id();
        return id;
    }
    
    using ComponentMask = std::bitset<MAX_COMPONENETS>;
}
