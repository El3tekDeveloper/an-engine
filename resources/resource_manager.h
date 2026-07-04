#pragma once
#include "core/debug/logger.h"
#include "core/utils/uuid.h"
#include "resources/texture.h"
#include "resources/material.h"
#include <memory>
#include <string>

class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    static ResourceManager& get_singleton() {
        static ResourceManager instance;
        return instance;
    }
    
    Texture* get_texture(const std::string& path) {
        UUID id = make_uuid(path);
        
        auto it = textures.find(id);
        if (it != textures.end()) {
            return it->second.get();
        }

        auto texture = create_texture_impl(path);
        if (!texture || !texture->load(path)) {
            LOG_ERROR("Failed to Get/Create Texture '{}'", path);
            return nullptr;
        }

        Texture* raw = texture.get();
        textures.emplace(id, std::move(texture));
        return raw;
    }

    Material* create_material() {
        auto mat = create_material_impl();
        UUID id = make_uuid("material_" + std::to_string(material_counter++));
        mat->uuid = id;

        Material* raw = mat.get();
        materials.emplace(id, std::move(mat));
        return raw;
    }

    Texture* find_texture (UUID id) { return find_in(textures,  id); }
    Material* find_material (UUID id) { return find_in(materials, id); }

    void destroy_all() {
        textures.clear();
        materials.clear();
    }

    void set_impls(std::function<std::unique_ptr<Texture>()> texture_impl_func) {
        texture_impl = std::move(texture_impl_func);
    }

private:
    std::unordered_map<UUID, std::unique_ptr<Texture>>  textures;
    std::unordered_map<UUID, std::unique_ptr<Material>> materials;
    uint32_t material_counter = 1;

    std::function<std::unique_ptr<Texture>()> texture_impl  = []{ return std::make_unique<Texture>(); };

    template<typename T>
    T* find_in(std::unordered_map<UUID, std::unique_ptr<T>>& map, UUID id) {
        auto it = map.find(id);
        return it != map.end() ? it->second.get() : nullptr;
    }

    std::unique_ptr<Texture> create_texture_impl(const std::string& path) {
        return texture_impl();
    }
    std::unique_ptr<Material> create_material_impl() {
        return std::make_unique<Material>();
    }
};

static ResourceManager& ResourceManager = ResourceManager::get_singleton();
