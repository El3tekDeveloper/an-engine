#pragma once
#include "resources/script.h"
#include "component.h"

struct [[export]] ScriptComponent : public Component {
    std::unique_ptr<Script> instance;

    template <typename T, typename... Args>
    static ScriptComponent create(Args&&... args) {
        ScriptComponent c;
        c.instance = std::make_unique<T>(std::forward<Args>(args)...);
        return c;
    }
};
