#pragma once
#include "core/debug/logger.h"
#include "core/utils/demangle.h"
#include <any>
#include <cstddef>
#include <format>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

#define export        clang::annotate("reflect-export")
#define range(mn, mx) clang::annotate("reflect-range;" #mn "," #mx)
#define button        clang::annotate("reflect-button")

enum class TypeKind {
    Primitive,
    Class,
    Enum,
    Vector,
    Array
};

using ToStringFn = std::string (*)(const void*);

class Type {
public:
    Type() = default;
    constexpr Type(std::string name, size_t size, ToStringFn to_string = nullptr, TypeKind kind = TypeKind::Primitive, Type* element = nullptr)
        : m_name(name), m_size(size), to_string(to_string), m_kind(kind), m_element_type(element) {}

    std::string get_name() const { return m_name; }
    size_t get_size() const { return m_size; }

    ToStringFn get_to_string() const { return to_string; }

    TypeKind get_kind() const { return m_kind; }
    Type* get_element_type() const { return m_element_type; }

    std::string to_string_value(const void* ptr) const {
        return to_string ? to_string(ptr) : "";
    }

    void set_name(std::string_view value) { m_name = value; }
    void set_kind(TypeKind value) { m_kind = value; }

    bool is_valid() const { return !m_name.empty() && m_size != 0; }

private:
    std::string m_name;
    size_t m_size = -1;
    ToStringFn to_string = nullptr;
    TypeKind m_kind = TypeKind::Primitive;
    Type* m_element_type = nullptr;
};

template<typename T>
std::string to_string_data_type(const void* p) {
    return std::to_string(*static_cast<const T*>(p));
}

extern std::unordered_map<std::type_index, Type> KNOWN_TYPES;

template<typename T>
inline void register_type(std::string name, ToStringFn to_string = nullptr, TypeKind kind = TypeKind::Primitive, Type* element = nullptr) {
    KNOWN_TYPES.emplace(
        typeid(T),
        Type{name, sizeof(T), to_string, kind, element}
    );
}

template<typename T>
struct type_traits {
    static constexpr TypeKind kind = TypeKind::Primitive;
};

template<typename T, typename Alloc>
struct type_traits<std::vector<T, Alloc>> {
    static constexpr TypeKind kind = TypeKind::Vector;
    using element_type = T;
};

template<typename T, size_t N>
struct type_traits<std::array<T, N>> {
    static constexpr TypeKind kind = TypeKind::Array;
    using element_type = T;
    static constexpr size_t size = N;
};

template<typename T>
using base_type_t = std::remove_cv_t<std::remove_pointer_t<std::remove_reference_t<T>>>;

template<typename T>
Type* get_type() {
    using TypeT = std::remove_cv_t<std::remove_reference_t<T>>;
    
    if constexpr (std::is_pointer_v<TypeT>) {
        using Element = std::remove_pointer_t<TypeT>;

        ToStringFn to_string = [](const void* p) -> std::string {
            auto ptr = *static_cast<Element* const*>(p);

            if (!ptr)
                return "null";

            Type* element_type = get_type<Element>();

            if (!element_type)
                return "<invalid>";

            if (element_type->get_to_string())
                return element_type->to_string_value(ptr);

            return std::format("{}@{}", element_type->get_name(), static_cast<const void*>(ptr));
        };

        static Type type{
            std::string(get_type<Element>()->get_name()) + "*",
            sizeof(TypeT),
            to_string,
            get_type<Element>()->get_kind(),
            get_type<Element>()
        };

        return &type;
    }

    if constexpr (type_traits<TypeT>::kind == TypeKind::Vector) {
        using Element = typename type_traits<TypeT>::element_type;

        ToStringFn to_string = [](const void* p) {
            const auto& vec = *static_cast<const TypeT*>(p);

            std::string out = "[";

            for (size_t i = 0; i < vec.size(); ++i) {
                if (i)
                    out += ", ";

                out += get_type<Element>()->to_string_value(&vec[i]);
            }

            out += "]";
            return out;
        };

        static Type type{
            std::format("std::vector<{}>", get_type<Element>()->get_name()),
            sizeof(TypeT),
            to_string,
            TypeKind::Vector,
            get_type<Element>()
        };

        return &type;
    }

    if constexpr (type_traits<TypeT>::kind == TypeKind::Array) {
        using Element = typename type_traits<TypeT>::element_type;
        constexpr size_t Count = type_traits<TypeT>::size;

        ToStringFn to_string = [](const void* p) {
            const auto& arr = *static_cast<const TypeT*>(p);

            std::string out = "[";

            for (size_t i = 0; i < Count; ++i) {
                if (i)
                    out += ", ";

                out += get_type<Element>()->to_string_value(&arr[i]);
            }

            out += "]";

            return out;
        };

        static Type type{
            std::format("std::array<{}, {}>",
                get_type<Element>()->get_name(),
                Count),
            sizeof(TypeT),
            to_string,
            TypeKind::Array,
            get_type<Element>()
        };

        return &type;
    }

    if constexpr (std::is_enum_v<TypeT>) {
        using Underlying = std::underlying_type_t<TypeT>;

        static Type type{
            type_name<TypeT>(),
            sizeof(TypeT),
            nullptr,
            TypeKind::Enum,
            get_type<Underlying>()
        };

        return &type;
    }

    auto it = KNOWN_TYPES.find(typeid(TypeT));
    if (it == KNOWN_TYPES.end()) {
        LOG_ERROR("Type '{}' is not registered", type_name<TypeT>());
        return nullptr;
    }

    return &it->second;
}

inline Type* get_type_by_name(std::string_view name) {
    for (auto& [idx, t] : KNOWN_TYPES)
        if (!t.get_name().empty() && std::string_view(t.get_name()) == name)
            return &t;
    return nullptr;
}

using MetaMap = std::unordered_map<std::string, std::any>;

struct Field {
    Type* type = nullptr;
    std::string name;
    size_t offset = -1;
    
    bool has_range = false;
    double range_min = 0.0;
    double range_max = 0.0;
    
    MetaMap meta = {};

    bool is_valid() const {
        return type != nullptr
            && !name.empty()
            && offset != static_cast<std::size_t>(-1);
    }
};

template<typename T>
const T* get_meta(const MetaMap& meta, std::string_view key) {
    auto it = meta.find(std::string(key));
    if (it == meta.end()) return nullptr;
    return std::any_cast<T>(&it->second);
}

using InvokeFn = void (*)(void* instance, void** args, void* ret);

struct RefFunction {
    Field return_value = {};
    std::vector<Field> parameters = {};

    std::string name;
    
    InvokeFn invoke = nullptr;
    MetaMap meta = {};

    bool is_valid() const { return !name.empty(); }
};

class TypeClass : public Type {
public:
    TypeClass() = default;
    TypeClass(std::string name, std::size_t size)
        : Type(name, size) { set_kind(TypeKind::Class); }
 
    std::vector<Field>& get_fields() { return m_fields; }
    std::vector<RefFunction>& get_functions() { return m_functions; }
    MetaMap& get_meta() { return m_meta; }
 
    const Field* find_field(const char* field_name) const {
        for (auto& f : m_fields)
            if (!f.name.empty() && std::string_view(f.name) == field_name)
                return &f;
        return nullptr;
    }
 
    const RefFunction* find_function(const char* fn_name) const {
        for (auto& fn : m_functions)
            if (!fn.name.empty() && std::string_view(fn.name) == fn_name)
                return &fn;
        return nullptr;
    }
 
    TypeClass* (*get_impl)() = nullptr;
    void* (*create_instance)() = nullptr;
    TypeClass* parent_type = nullptr;
 
    TypeClass* get() {
        return get_impl ? get_impl() : this;
    }
 
    bool is_a(const TypeClass* other) const {
        const TypeClass* cur = this;
        while (cur) {
            if (cur == other) return true;
            cur = cur->parent_type;
        }
        return false;
    }
 
private:
    std::vector<Field> m_fields;
    std::vector<RefFunction> m_functions;
    MetaMap m_meta;
};

template<typename T>
TypeClass* get_class_impl();

template<typename T>
inline TypeClass* get_class() {
    return get_class_impl<T>();
}

inline std::unordered_map<std::string_view, TypeClass*> CLASS_REGISTRY;
inline TypeClass* get_class_by_name(std::string_view name) {
    auto it = CLASS_REGISTRY.find(name);
    return it != CLASS_REGISTRY.end() ? it->second : nullptr;
}
