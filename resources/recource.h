#pragma once
#include "core/utils/uuid.h"

class Resource {
public:
    virtual ~Resource() = default;

    UUID get_uuid() const { return uuid; }
    void set_uuid(const UUID& value) { uuid = value; }

protected:
    UUID uuid = UUID_INVALID;

    friend class ResourceManager;
};
