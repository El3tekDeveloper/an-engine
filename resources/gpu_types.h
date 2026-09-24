#pragma once
#include "core/math/color.h"
#include "core/math/vector2.h"
#include "core/math/vector2i.h"
#include "core/math/vector3.h"
#include "core/math/vector3i.h"
#include "core/math/vector4.h"
#include "tools/reflector/runtime/type_registry.h"
#include <cstddef>
#include <cstdint>
#include <format>
#include <string>

struct GpuTypeInfo {
    std::string name;
    size_t size = 0;
    size_t alignment = 0;
};

inline const std::unordered_map<Type*, GpuTypeInfo>& gpu_types() {
    static const std::unordered_map<Type*, GpuTypeInfo> types = {
        {get_type<int>(),      {"int", 4, 4}},
        {get_type<float>(),    {"float", 4, 4}},
        {get_type<bool>(),     {"bool", 4, 4}},
        {get_type<uint32_t>(), {"uint", 4, 4}},

        {get_type<Vector2>(),  {"vec2", 8, 8}},
        {get_type<Vector2i>(), {"vec2i", 8, 8}},
        {get_type<Vector3>(),  {"vec3", 12, 16}},
        {get_type<Vector3i>(), {"vec3i", 12, 16}},
        {get_type<Vector4>(),  {"vec4", 16, 16}},
        {get_type<Color>(),    {"vec4", 16, 16}},
    };
    return types;
}

class GpuHandle : public TypeClass {
public:
    GpuHandle() = default;
    explicit GpuHandle(std::string name) {
        m_name = std::move(name);
    }

    ~GpuHandle() = default;

    template<typename T>
    Field& add_filed(std::string_view name) {
        Type* type = get_type<T>();
        if (!type)
            LOG_ERROR("GpuHandle: type for field '{}' is not registered", name);

        auto it = gpu_types().find(type);
        if (it == gpu_types().end()) {
            LOG_ERROR("GpuHandle: type '{}' for field '{}' has no GPU representation", type ? type->get_name() : "?", name);

            Field f;
            f.name = std::string(name);
            f.type = type;
            m_fields.push_back(std::move(f));
            return m_fields.back();
        }
        const GpuTypeInfo& info = it->second;

        size_t aligned_offset = (m_current_offset + info.alignment - 1) & ~(info.alignment - 1);

        Field f;
        f.name = std::string(name);
        f.type = type;
        f.offset = aligned_offset;

        m_current_offset = aligned_offset + info.size;

        m_fields.push_back(std::move(f));
        return m_fields.back();
    }

    template<typename C, typename T>
    Field& add_filed(T C::* member, std::string_view name) {
        Type* type = get_type<T>();
        if (!type)
            LOG_ERROR("GpuHandle: type for field '{}' is not registered", name);

        size_t offset = reinterpret_cast<size_t>(
            &(reinterpret_cast<C*>(0)->*member)
        );

        auto it = gpu_types().find(type);
        if (it == gpu_types().end()) {
            LOG_ERROR("GpuHandle: type '{}' for field '{}' has no GPU representation", type ? type->get_name() : "?", name);

            Field f;
            f.name = std::string(name);
            f.type = type;
            f.offset = offset;
            m_fields.push_back(std::move(f));
            return m_fields.back();
        }

        const GpuTypeInfo& info = it->second;

        size_t aligned_offset = (m_current_offset + info.alignment - 1) & ~(info.alignment - 1);
        m_current_offset = aligned_offset + info.size;

        Field f;
        f.name = std::string(name);
        f.type = type;
        f.offset = offset;

        m_fields.push_back(std::move(f));
        return m_fields.back();
    }

    size_t get_total_size() const { return m_current_offset; }

    std::string get_string() const {
        std::string s;
        s += std::format("struct {} {{\n", m_name);

        for (auto& f : m_fields) {
            std::string gpu_type_name = f.type->get_name();

            if (auto it = gpu_types().find(f.type); it != gpu_types().end())
                gpu_type_name = it->second.name;

            s += std::format("    {} {};\n", gpu_type_name, f.name);
        }

        s += "};\n";
        return s;
    }

private:
    size_t m_current_offset = 0;
};

#define ADD_GPU_FIELD(handle, Class, member) \
    (handle).add_filed(&Class::member, #member)
