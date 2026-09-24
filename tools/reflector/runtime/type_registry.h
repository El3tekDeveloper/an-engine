#pragma once
#include "core/debug/logger.h"
#include "core/utils/demangle.h"
#include "core/utils/string_utils.h"
#include <any>
#include <cstddef>
#include <format>
#include <map>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

#define export clang::annotate("reflect-export")
#define range(mn, mx) clang::annotate("reflect-range;" #mn "," #mx)
// #define button clang::annotate("reflect-button")

enum class TypeKind {
    Primitive,
    Class,
    Enum,
    Vector,
    Array,
    Map
};

using ToStringFn = std::string (*)(const void*);
using FromStringFn = bool (*)(std::string_view, void*);

using MetaMap = std::unordered_map<std::string, std::any>;
using InvokeFn = void (*)(void* instance, void** args, void* ret);

class Type {
public:
    Type() = default;
    constexpr Type(std::string name, size_t size,
        ToStringFn to_string = nullptr, FromStringFn from_string = nullptr,
        TypeKind kind = TypeKind::Primitive, Type* element = nullptr)
        : m_name(name), m_size(size), to_string(to_string), from_string(from_string), m_kind(kind), m_element_type(element) {}

    std::string get_name() const { return m_name; }
    size_t get_size() const { return m_size; }

    ToStringFn get_to_string() const { return to_string; }
    FromStringFn get_from_string() const { return from_string; }

    TypeKind get_kind() const { return m_kind; }
    Type* get_element_type() const { return m_element_type; }

    std::string to_string_value(const void* ptr) const {
        return to_string ? to_string(ptr) : "";
    }

    bool from_string_value(std::string_view str, void* ptr) const {
        return from_string ? from_string(str, ptr) : false;
    }

    void set_name(std::string_view value) { m_name = value; }
    void set_kind(TypeKind value) { m_kind = value; }

    bool is_valid() const { return !m_name.empty() && m_size != 0; }

    bool is_primitive() const { return m_kind == TypeKind::Primitive; }
    bool is_class() const { return m_kind == TypeKind::Class; }
    bool is_enum() const { return m_kind == TypeKind::Enum; }
    bool is_vector() const { return m_kind == TypeKind::Vector; }
    bool is_array() const { return m_kind == TypeKind::Array; }
    bool is_map() const { return m_kind == TypeKind::Map; }

    virtual Type* get_parent() const { return nullptr; }
    virtual ~Type() = default;

protected:
    std::string m_name;

private:
    size_t m_size = -1;

protected:
    ToStringFn to_string = nullptr;
    FromStringFn from_string = nullptr;

    TypeKind m_kind = TypeKind::Primitive;
    Type* m_element_type = nullptr;
};

std::unordered_map<std::type_index, Type*>& known_types();

template<typename T>
inline void register_type(
    std::string name,
    ToStringFn to_string = nullptr,
    FromStringFn from_string = nullptr,
    TypeKind kind = TypeKind::Primitive,
    Type* element = nullptr) {
    
    static Type type{name, sizeof(T), to_string, from_string, kind, element};
    known_types().emplace(typeid(T), &type);
}

template<typename T>
inline void register_type(Type* type) {
    known_types().emplace(typeid(T), type);
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

template<typename K, typename V, typename Compare, typename Alloc>
struct type_traits<std::map<K, V, Compare, Alloc>> {
    static constexpr TypeKind kind = TypeKind::Map;
    using key_type = K;
    using mapped_type = V;
    static constexpr std::string_view name_prefix = "std::map";
};

template<typename K, typename V, typename Hash, typename KeyEqual, typename Alloc>
struct type_traits<std::unordered_map<K, V, Hash, KeyEqual, Alloc>> {
    static constexpr TypeKind kind = TypeKind::Map;
    using key_type = K;
    using mapped_type = V;
    static constexpr std::string_view name_prefix = "std::unordered_map";
};

template<typename T>
using base_type_t = std::remove_cv_t<std::remove_pointer_t<std::remove_reference_t<T>>>;

struct Field {
    Type* type = nullptr;
    std::string name;
    size_t offset = -1;
    
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
        : Type(name, size) { m_kind = TypeKind::Class; }
 
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

    Type* get_parent() const override { return parent_type; }
 
protected:
    std::vector<Field> m_fields;
    std::vector<RefFunction> m_functions;
    MetaMap m_meta;
};

template<typename T>
struct has_reflection : std::false_type {};

template<typename T>
TypeClass* get_class_impl();
 
template<typename T>
inline TypeClass* get_class() {
    return get_class_impl<T>();
}

template<typename T>
Type* get_type() {
    using TypeT = std::remove_cv_t<std::remove_reference_t<T>>;
    
    if constexpr (std::is_pointer_v<TypeT>) {
        using Element = std::remove_pointer_t<TypeT>;

        ToStringFn to_string = [](const void* p) -> std::string {
            auto ptr = *static_cast<Element* const*>(p);
            if (!ptr) return "null";

            Type* element_type = get_type<Element>();
            if (!element_type)
                return "<invalid>";
            if (element_type->get_to_string())
                return element_type->to_string_value(ptr);
            
            return std::format("{}@{}", element_type->get_name(), static_cast<const void*>(ptr));
        };
        FromStringFn from_string = [](std::string_view str, void* p) -> bool {
            if (str == "null" || str.empty()) {
                *static_cast<Element**>(p) = nullptr;
                return true;
            }
            return false;
        };

        static Type type{
            std::string(get_type<Element>()->get_name()) + "*",
            sizeof(TypeT),
            to_string,
            from_string,
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
                if (i) out += ", ";
                out += get_type<Element>()->to_string_value(&vec[i]);
            }
            out += "]";
            return out;
        };
        FromStringFn from_string = [](std::string_view str, void* p) -> bool {
            auto& vec = *static_cast<TypeT*>(p);
            
            if (str.size() < 2 || str.front() != '[' || str.back() != ']')
                return false;

            std::string_view inner = str.substr(1, str.size() - 2);
            vec.clear();
            
            if (inner.find_first_not_of(' ') == std::string_view::npos)
                return true;
            
            Type* element_type = get_type<Element>();
            if (!element_type)
                return false;

            for (auto part : split_top_level(inner)) {
                Element elem{};
                if (!element_type->from_string_value(part, &elem))
                    return false;

                vec.push_back(std::move(elem));
            }
            return true;
        };

        static Type type{
            std::format("std::vector<{}>", get_type<Element>()->get_name()),
            sizeof(TypeT),
            to_string,
            from_string,
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
        FromStringFn from_string = [](std::string_view str, void* p) -> bool {
            auto& arr = *static_cast<TypeT*>(p);

            if (str.size() < 2 || str.front() != '[' || str.back() != ']')
                return false;

            std::string_view inner = str.substr(1, str.size() - 2);
            auto parts = split_top_level(inner);

            if (parts.size() != Count)
                return false;

            Type* element_type = get_type<Element>();
            if (!element_type)
                return false;

            for (size_t i = 0; i < Count; ++i) {
                if (!element_type->from_string_value(parts[i], &arr[i]))
                    return false;
            }
            return true;
        };

        static Type type{
            std::format("std::array<{}, {}>",
                get_type<Element>()->get_name(),
                Count),
            sizeof(TypeT),
            to_string,
            from_string,
            TypeKind::Array,
            get_type<Element>()
        };

        return &type;
    }

    if constexpr (type_traits<TypeT>::kind == TypeKind::Map) {
        using Key   = typename type_traits<TypeT>::key_type;
        using Value = typename type_traits<TypeT>::mapped_type;

        Type* key_type_ptr   = get_type<Key>();
        Type* value_type_ptr = get_type<Value>();

        if (!key_type_ptr || !value_type_ptr) {
            LOG_ERROR("Cannot reflect map type: key or value type not registered");
            return nullptr;
        }

        ToStringFn to_string = [](const void* p) {
            const auto& map = *static_cast<const TypeT*>(p);
            std::string out = "[";
            bool first = true;
            for (auto& [k, v] : map) {
                if (!first) out += ", ";
                first = false;
                out += get_type<Key>()->to_string_value(&k);
                out += ": ";
                out += get_type<Value>()->to_string_value(&v);
            }
            out += "]";
            return out;
        };
        FromStringFn from_string = [](std::string_view str, void* p) -> bool {
            auto& map = *static_cast<TypeT*>(p);

            if (str.size() < 2 || str.front() != '[' || str.back() != ']')
                return false;

            std::string_view inner = str.substr(1, str.size() - 2);

            map.clear();

            if (inner.find_first_not_of(' ') == std::string_view::npos)
                return true;

            Type* key_type = get_type<Key>();
            Type* value_type = get_type<Value>();
            if (!key_type || !value_type)
                return false;

            for (auto part : split_top_level(inner)) {
                size_t depth = 0;
                size_t colon_pos = std::string_view::npos;

                for (size_t i = 0; i < part.size(); ++i) {
                    char c = part[i];
                    if (c == '[')
                        ++depth;
                    else if (c == ']')
                        --depth;
                    else if (c == ':' && depth == 0) {
                        colon_pos = i;
                        break;
                    }
                }

                if (colon_pos == std::string_view::npos)
                    return false;

                std::string_view key_str = part.substr(0, colon_pos);
                std::string_view value_str = part.substr(colon_pos + 1);

                while (!key_str.empty() && key_str.front() == ' ') key_str.remove_prefix(1);
                while (!key_str.empty() && key_str.back() == ' ')  key_str.remove_suffix(1);
                while (!value_str.empty() && value_str.front() == ' ') value_str.remove_prefix(1);
                while (!value_str.empty() && value_str.back() == ' ')  value_str.remove_suffix(1);

                Key key{};
                Value value{};

                if (!key_type->from_string_value(key_str, &key))
                    return false;
                if (!value_type->from_string_value(value_str, &value))
                    return false;

                map.emplace(std::move(key), std::move(value));
            }
            return true;
        };

        static Type type{
            std::format("{}<{}, {}>",
                type_traits<TypeT>::name_prefix,
                key_type_ptr->get_name(),
                value_type_ptr->get_name()),
            sizeof(TypeT),
            to_string,
            from_string,
            TypeKind::Map,
            value_type_ptr
        };
        return &type;
    }

    if constexpr (std::is_enum_v<TypeT>) {
        using Underlying = std::underlying_type_t<TypeT>;

        FromStringFn from_string = [](std::string_view str, void* p) -> bool {
            Underlying value{};
            if (!get_type<Underlying>()->from_string_value(str, &value))
                return false;
            *static_cast<TypeT*>(p) = static_cast<TypeT>(value);
            return true;
        };
        
        static Type type{
            type_name<TypeT>(),
            sizeof(TypeT),
            nullptr,
            from_string,
            TypeKind::Enum,
            get_type<Underlying>()
        };
        return &type;
    }

    auto it = known_types().find(typeid(TypeT));
    if (it != known_types().end())
        return it->second;

    if constexpr (has_reflection<TypeT>::value) {
        TypeClass* cls = get_class_impl<TypeT>();
        if (cls) {
            known_types().emplace(typeid(TypeT), cls);
            return cls;
        }
    }

    LOG_ERROR("Type '{}' is not registered", type_name<TypeT>());
    return nullptr;
}

inline Type* get_type_by_name(std::string_view name) {
    for (auto& [idx, t] : known_types())
        if (t && !t->get_name().empty() && std::string_view(t->get_name()) == name)
            return t;
    return nullptr;
}

inline std::vector<Type*> get_types_by_kind(TypeKind kind) {
    std::vector<Type*> result;
    for (auto& [idx, t] : known_types())
        if (t && t->get_kind() == kind)
            result.push_back(t);
    return result;
}
