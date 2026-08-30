#pragma once
#include "core/math/color.h"
#include "core/math/vector2.h"
#include "resources/gpu_types.h"
#include "resources/resource.h"
#include "resources/texture.h"
#include <cstdint>

#define MATERIAL_SLOT_ALBEDO     0
#define MATERIAL_SLOT_NORMAL     1
#define MATERIAL_SLOT_METALROUGH 2
#define MATERIAL_SLOT_EMISSIVE   3

#define MATERIAL_HAS_ALBEDO_TEX     (1 << 0)
#define MATERIAL_HAS_NORMAL_TEX     (1 << 1)
#define MATERIAL_HAS_METALROUGH_TEX (1 << 2)
#define MATERIAL_HAS_EMISSIVE_TEX   (1 << 3)
#define MATERIAL_DOUBLE_SIDED       (1 << 4)
#define MATERIAL_UNLIT              (1 << 5)

class Material : public Resource {
public:
    Material() = default;
    ~Material() = default;

    Texture* albedo_texture;
    Texture* normal_texture;
    Texture* metalrough_texture;
    Texture* emissive_texture;

    Color albedo_factor = Color::White;
    Color emissive_factor = Color::Black;
    Vector2 uv_scale = Vector2::One;
    Vector2 uv_offset = Vector2::Zero;
    Vector2 metallic_roughness = Vector2::Down;
    alignas(4) uint32_t flags = 0;

    static constexpr size_t gpu_size() {
        return sizeof(Material) - offsetof(Material, albedo_factor);
    }
    
    bool set_albedo(Texture* texture)
        { return assign_texture(albedo_texture, texture, MATERIAL_HAS_ALBEDO_TEX); }
    bool set_normal(Texture* texture)
        { return assign_texture(normal_texture, texture, MATERIAL_HAS_NORMAL_TEX); }
    bool set_metalrough(Texture* texture)
        { return assign_texture(metalrough_texture, texture, MATERIAL_HAS_METALROUGH_TEX); }
    bool set_emissive(Texture* texture)
        { return assign_texture(emissive_texture, texture, MATERIAL_HAS_EMISSIVE_TEX); }

    static const GpuHandle& get_gpu_handle() {
        static GpuHandle handle = [] {
            GpuHandle h("Material");
            ADD_GPU_FIELD(h, Material, albedo_factor);
            ADD_GPU_FIELD(h, Material, emissive_factor);
            ADD_GPU_FIELD(h, Material, uv_scale);
            ADD_GPU_FIELD(h, Material, uv_offset);
            ADD_GPU_FIELD(h, Material, metallic_roughness);
            ADD_GPU_FIELD(h, Material, flags);
            return h;
        }();
        return handle;
    }
    
private:
    bool assign_texture(Texture*& slot, Texture* texture, uint32_t flag) {
        if (!texture) return false;
        slot = static_cast<Texture*>(texture);
        flags |= flag;
        return true;
    }
};
