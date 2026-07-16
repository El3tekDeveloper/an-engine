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

layout(std140, binding = 2) uniform material_ubo {
    Material material;
};

uniform sampler2D texture_albedo;
uniform sampler2D texture_normal;
uniform sampler2D texture_metalrough;
uniform sampler2D texture_emissive;

layout(location = 0) out vec4 frag_color;

in vec2 texture_coords;
in vec4 vertex_color;
in vec3 frag_position;
in vec3 frag_normal;
in vec3 frag_tangent;

void main() {
    vec2 uv = texture_coords * material.uv_scale + material.uv_offset;

    // Albedo
    vec4 albedo = material.albedo_factor;
    if ((material.flags & uint(MATERIAL_HAS_ALBEDO_TEX)) != 0u) {
        albedo *= texture(texture_albedo, uv);
    }
    if (albedo.a == 0.0) discard;

    // Normal mapping
    vec3 N = normalize(frag_normal);
    if ((material.flags & uint(MATERIAL_HAS_NORMAL_TEX)) != 0u) {
        vec3 T   = normalize(frag_tangent);
        vec3 B   = cross(N, T);
        mat3 TBN = mat3(T, B, N);
        vec3 n   = texture(texture_normal, uv).rgb * 2.0 - 1.0;
        N = normalize(TBN * n);
    }

    // Test Lighting
    vec3 ambient = 0.3 * vec3(1.0);

    vec3 light_pos = vec3(1.0, 1.0, 1.0);
    vec3 light_dir = normalize(light_pos - frag_position);

    float diff = max(dot(N, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    vec2 metalrough = material.metallic_roughness;
    if ((material.flags & uint(MATERIAL_HAS_METALROUGH_TEX)) != 0u) {
        vec2 mr_sample = texture(texture_metalrough, uv).bg;
        metalrough *= mr_sample;
    }
    float metallic  = metalrough.x;
    float roughness = metalrough.y;

    // Emissive
    vec3 emissive = material.emissive_factor.rgb;
    if ((material.flags & uint(MATERIAL_HAS_EMISSIVE_TEX)) != 0u) {
        emissive *= texture(texture_emissive, uv).rgb;
    }

    vec3 color = (ambient + diffuse) * albedo.rgb * vertex_color.rgb;
    color += emissive;

    frag_color = vec4(color, albedo.a * vertex_color.a);
}
