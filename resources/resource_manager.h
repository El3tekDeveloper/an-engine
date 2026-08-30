#pragma once
#include "core/debug/logger.h"
#include "core/io/path.h"
#include "core/utils/uid.h"
#include "resources/resource.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <type_traits>
#include <vector>

using ResourceHandle = UID;

template <typename T, typename = void>
struct is_loadable : std::false_type {};

template <typename T>
struct is_loadable<T, std::void_t<decltype(std::declval<T>().load(std::declval<const std::string&>()))>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_loadable_v = is_loadable<T>::value;

template <typename T>
class ResourceRegistry {
public:
    using Factory = std::function<std::unique_ptr<T>()>;

    void reserve(size_t capacity) {
        m_resources.reserve(capacity);
    }

    void set_factory(Factory factory) {
        m_factory = std::move(factory);
    }

    template <typename... Args>
    T* create(ResourceHandle* out_handle = nullptr, Args&&... args) {
        grow_if_needed();

        std::unique_ptr<T> resource = make_instance(std::forward<Args>(args)...);
        ResourceHandle handle = generate_handle();

        T* raw = resource.get();
        m_resources.emplace(handle, std::move(resource));

        if (out_handle) *out_handle = handle;
        return raw;
    }

    std::unique_ptr<T> make_default() {
        return m_factory ? m_factory() : std::make_unique<T>();
    }

    T* emplace(ResourceHandle handle, std::unique_ptr<T> resource) {
        grow_if_needed();
        T* raw = resource.get();
        m_resources[handle] = std::move(resource);
        return raw;
    }

    T* get(ResourceHandle handle) {
        auto it = m_resources.find(handle);
        return it != m_resources.end() ? it->second.get() : nullptr;
    }

    const T* get(ResourceHandle handle) const {
        auto it = m_resources.find(handle);
        return it != m_resources.end() ? it->second.get() : nullptr;
    }

    bool contains(ResourceHandle handle) const {
        return m_resources.find(handle) != m_resources.end();
    }

    bool release(ResourceHandle handle) {
        return m_resources.erase(handle) > 0;
    }

    void clear() {
        m_resources.clear();
    }

    size_t size() const { return m_resources.size(); }

private:
    std::unordered_map<ResourceHandle, std::unique_ptr<T>> m_resources;
    Factory m_factory = []() { return std::make_unique<T>(); };
    uint64_t counter = 1;

    template <typename... Args>
    std::unique_ptr<T> make_instance(Args&&... args) {
        if constexpr (sizeof...(Args) > 0) {
            return std::make_unique<T>(std::forward<Args>(args)...);
        } else {
            return m_factory ? m_factory() : std::make_unique<T>();
        }
    }

    ResourceHandle generate_handle() {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "res_%llu", static_cast<unsigned long long>(counter++));
        return make_uid(buf);
    }

    void grow_if_needed() {
        size_t size = m_resources.size();
        size_t bucket_count = m_resources.bucket_count();
        float load_factor = m_resources.max_load_factor();

        if (static_cast<float>(size + 1) > static_cast<float>(bucket_count) * load_factor * 0.9f) {
            m_resources.reserve((size + 1) * 2);
        }
    }
};

class ResourceManager {
public:
    static ResourceManager& get_singleton() {
        static ResourceManager instance;
        return instance;
    }

    template <typename T>
    static ResourceRegistry<T>& get_registry() {
        static ResourceRegistry<T> registry;
        static bool registered = [](){
            get_singleton().register_clear_callback<T>();
            return true;
        }();
        (void)registered;
        return registry;
    }

    template <typename T>
    T* load(const std::string& path) {
        ResourceRegistry<T>& registry = get_registry<T>();
        ResourceHandle handle = make_uid(IO::resolve_path(path));

        if (T* existing = registry.get(handle)) {
            return existing;
        }

        std::unique_ptr<T> resource = registry.make_default();
        if (!resource->load(path)) {
            LOG_ERROR("Failed to load resource '{}'", path);
            return nullptr;
        }

        return registry.emplace(handle, std::move(resource));
    }

    template <typename T, typename LoadFunc>
    T* load(const std::string& path, LoadFunc&& load_func) {
        ResourceRegistry<T>& registry = get_registry<T>();
        ResourceHandle handle = make_uid(path);

        if (T* existing = registry.get(handle)) {
            return existing;
        }

        std::unique_ptr<T> resource = load_func(path);
        if (!resource) {
            LOG_ERROR("Failed to load resource '{}'", path);
            return nullptr;
        }

        return registry.emplace(handle, std::move(resource));
    }

    template <typename T, typename... Args>
    T* create(ResourceHandle* out_handle = nullptr, Args&&... args) {
        return get_registry<T>().create(out_handle, std::forward<Args>(args)...);
    }

    template <typename T>
    T* get(ResourceHandle handle) {
        return get_registry<T>().get(handle);
    }

    template <typename T>
    bool release(ResourceHandle handle) {
        return get_registry<T>().release(handle);
    }

    template <typename T>
    void reserve(size_t capacity) {
        get_registry<T>().reserve(capacity);
    }

    template <typename T>
    void clear() {
        get_registry<T>().clear();
    }

    void clear_all() {
        for (auto& clear_func : clear_callbacks_) {
            clear_func();
        }
    }

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    std::vector<std::function<void()>> clear_callbacks_;

    template <typename T>
    void register_clear_callback() {
        clear_callbacks_.push_back([]() { get_registry<T>().clear(); });
    }
};

static ResourceManager& ResourceManager = ResourceManager::get_singleton();
