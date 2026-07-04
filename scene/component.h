#pragma once
#include "core/utils/demangle.h"
#include "tools/reflector/type_registry.h"
#include <boost/pfr/core_name.hpp>
#include <string>

class Entity;

class [[export]] Component {
public:
    virtual ~Component() = default;
    Entity* entity = nullptr;
    
    [[export]]
    std::string get_type_name() const {
        return type_name(*this);
    }
};

#if __REFLECT_GENERATED__
#include ".generated/component.generated.hxx"
#endif
