#pragma once
#include "core/utils/demangle.h"
#include <tools/reflector/type_registry.h>
#include <string>

struct [[export]] Component {
    virtual ~Component() = default;
    [[export]] bool active = true;
    
    virtual inline std::string get_type_name() const {
        return type_name(*this);
    }
};

#if __REFLECT_GENERATED__
#include ".generated/component.generated.hxx"
#endif
