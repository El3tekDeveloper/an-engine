#pragma once
#include "core/utils/demangle.h"
#include "scene2/ecs.h"
#include <tools/reflector/runtime/type_registry.h>
#include <string>

class Scene;

struct [[export]] Component {
    virtual ~Component() = default;

    [[export]] bool active = true;
    ecs::Entity owner = ecs::entity::INVALID;
    Scene* owner_scene;
    
    virtual inline std::string get_type_name() const {
        return type_name(*this);
    }
};

