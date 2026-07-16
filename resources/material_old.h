#ifdef __cplusplus
#pragma once
#include "resources/recource.h"
#include "resources/gpu_types.h"
#include "resources/texture.h"
#include <cstddef>
#endif

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

#ifdef __cplusplus
class Material : public Resource {
public:
#else
struct Material {
#endif
    GPU_COLOR albedo_factor      GPU_DEFAULT(GPU_COLOR_WHITE);
    GPU_COLOR emissive_factor    GPU_DEFAULT(GPU_MAKE_COLOR(0, 0, 0, 1));
    GPU_VEC2  uv_scale           GPU_DEFAULT(GPU_MAKE_VEC2(1, 1));
    GPU_VEC2  uv_offset          GPU_DEFAULT(GPU_MAKE_VEC2(0, 0));
    GPU_VEC2  metallic_roughness GPU_DEFAULT(GPU_MAKE_VEC2(0, 1));
    GPU_UINT  flags              GPU_DEFAULT(0);

#ifdef __cplusplus
    Material() = default;
    ~Material() = default;
    
    Texture* albedo_texture;
    Texture* normal_texture;
    Texture* metalrough_texture;
    Texture* emissive_texture;

    static constexpr size_t gpu_size() {
        return sizeof(Material) - offsetof(Material, albedo_factor);
    }
    
    bool set_albedo (Texture* texture)     { return assign_texture(albedo_texture, texture, MATERIAL_HAS_ALBEDO_TEX); }
    bool set_normal (Texture* texture)     { return assign_texture(normal_texture, texture, MATERIAL_HAS_NORMAL_TEX); }
    bool set_metalrough (Texture* texture) { return assign_texture(metalrough_texture, texture, MATERIAL_HAS_METALROUGH_TEX); }
    bool set_emissive (Texture* texture)   { return assign_texture(emissive_texture, texture, MATERIAL_HAS_EMISSIVE_TEX); }
    
private:
    bool assign_texture(Texture*& slot, Texture* texture, uint32_t flag) {
        if (!texture) return false;
        slot = static_cast<Texture*>(texture);
        flags |= flag;
        return true;
    }
#endif
};
