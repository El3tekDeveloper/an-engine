#pragma once
#include <array>
#include <bitset>

class Component;

namespace ECS {
    using ComponentType = std::size_t;
    constexpr ComponentType MAX_COMPONENETS = 64;

    inline ComponentType get_unique_id() {
        static ComponentType last = 0;
        return last++;
    }

    template<typename T>
    ComponentType type_id() {
        static ComponentType id = get_unique_id();
        return id;
    }
    
    using ComponentBitset = std::bitset<MAX_COMPONENETS>;
    using ComponentList = std::array<Component*, MAX_COMPONENETS>;
}
