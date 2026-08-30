#pragma once
#include "core/debug/logger.h"
#include "core/io/file_access.h"
#include "core/utils/string_utils.h"
#include "core/utils/uid.h"
#include "tools/reflector/type_registry.h"
#include <sstream>
#include <string>
#include <tools/reflector/type_registry.h>

class Resource {
public:
    virtual ~Resource() = default;

    UID get_uid() const { return uid; }
    void set_uuid(const UID& value) { uid = value; }
    
    template<typename T>
    void save(const std::string& path);
    
    template<typename T>
    T& load(const std::string& path);

protected:
    UID uid = UID_INVALID;

    friend class ResourceManager;
};

template<typename T>
void Resource::save(const std::string& path) {
    TypeClass* res = get_class<T>();

    const T* obj = static_cast<const T*>(this);
    std::ostringstream out;
    
    if (uid == UID_INVALID) uid = make_uid(path);
    out << "[resource uid=\"" << uid << "\"]\n\n";

    if (res) {
        for (Field& field : res->get_fields()) {
            if (!field.is_valid()) continue;

            const void* field_ptr =
                reinterpret_cast<const std::byte*>(obj) + field.offset;
            std::string value = field.type->to_string_value(field_ptr);

            out << std::format("{} = {}\n", field.name, value);
        }
    }
    out << "\n";

    IO::write_file(path, out.str());
}

template<typename T>
T& Resource::load(const std::string& path) {
    TypeClass* res = get_class<T>();
    T* obj = static_cast<T*>(this);

    std::string content = IO::read_file(path);
    std::istringstream in(content);
    std::string line;
    
    // Set UID
    if (std::getline(in, line)) {
        size_t first_quot = line.find('"');
        size_t last_quot = line.rfind('"');

        if (first_quot != std::string::npos
            && last_quot != std::string::npos
            && last_quot > first_quot) {
            std::string uid_str = line.substr(first_quot, last_quot - first_quot - 1);
            uid = make_uid(uid_str);
        }
    }
    
    while (std::getline(in, line)) {
        size_t equal = line.find('=');
        if (equal == std::string::npos)
            continue;

        std::string name = trim(line.substr(0, equal));
        std::string value = trim(line.substr(equal + 1));

        Field* matched_field = nullptr;
        for (auto& field : res->get_fields()) {
            if (field.name == name) {
                matched_field = &field;
                break;
            }
        }

        if (!matched_field) continue;
        void* field_ptr = reinterpret_cast<std::byte*>(obj) + matched_field->offset;
        
        if (matched_field->type && matched_field->type->get_from_string()) {
            if (!matched_field->type->from_string_value(value, field_ptr)) {
                LOG_ERROR(
                    "Failed to parse field '{}' with value '{}' in resource '{}'",
                    name, value, path
                );
            }
        }
    }

    return *obj;
}

