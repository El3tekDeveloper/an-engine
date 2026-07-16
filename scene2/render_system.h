#pragma once
#include <algorithm>
#include <limits>
#include "core/math/matrix4.h"
#include "core/math/quaternion.h"
#include "core/math/vector3.h"
#include "render/renderer.h"
#include "resources/mesh.h"
#include "component.h"
#include "scene2/system.h"
#include "scene.h"
#include "registry.h"
#include "tools/reflector/type_registry.h"
#include "resources/sprite.h"

struct [[export]] Transform : public Component {
    [[export]] Vector3 position = Vector3::Zero;
    [[export]] Quaternion rotation = Quaternion::Identity;
    [[export]] Vector3 scale = Vector3::One;

private:
    Matrix4 get_matrix() const {
        Matrix4 T = Matrix4::translation(position);
        Matrix4 R = rotation.normalized().to_matrix();
        Matrix4 S = Matrix4::scale(scale);

        return T * R * S;
    }

    friend class RenderSystem;
};

enum class CameraType {
    Perspective,
    Orthographic
};

struct [[export]] Camera : public Component {
    [[export]] CameraType type = CameraType::Perspective;
    [[export, range(1, 179)]]
    float fov = 60.0f;
    
    [[export, range(0.001, 1000)]]
    float orthographic_size = 5.0f;

    [[export, range(0.001, 100)]]
    float near_plane = 0.1f;
    [[export, range(1, 10000)]]
    float far_plane = 100.0f;
};

struct [[export]] MeshRenderer : public Component {
    MeshRenderer() = default;
    explicit MeshRenderer(Mesh& mesh, const std::vector<Material*> materials = {})
        : mesh(&mesh), materials(materials) {}

    [[export]] Mesh* mesh = &Mesh::Cube;
    [[export]] std::vector<Material*> materials = {};
    
    [[export]] bool is_static = false;
    
    [[export]]
    void set_material(int index, Material* mat) {
        if (index >= 0 && index < static_cast<int>(materials.size()))
            materials[index] = mat;
    }

    [[export]]
    Material* get_material(int index) {
        if (index >= 0 && index < static_cast<int>(materials.size()))
            return materials[index];
        return nullptr;
    }

    [[export]]
    void add_material(Material* mat) {
        materials.push_back(mat);
    }

private:
    mutable Matrix4 cached_matrix = Matrix4::translation(Vector3::Zero);
    mutable bool cache_valid = false;

    friend class RenderSystem;
};

struct [[export]] SpriteRenderer : public Component {
    SpriteRenderer() = default;
    explicit SpriteRenderer(Sprite& sprite) : sprite(&sprite) {}
 
    [[export]] Sprite* sprite = nullptr;
    [[export]] Color color = Color::White;
 
    [[export]] bool flip_x = false;
    [[export]] bool flip_y = false;
 
    [[export]] int layer = 0;
};

class RenderSystem : public System {
public:
    virtual ~RenderSystem() = default;
    
    CameraData camera_data;

    void draw(Scene& scene, ViewPort& viewport, RenderData& rd) override {
        Transform* camera_transform = nullptr;
        Camera* active_camera = nullptr;

        for (auto entity : scene.get_registry().view<Transform, Camera>()) {
            active_camera = &scene.get_component<Camera>(entity);
            camera_transform = &scene.get_component<Transform>(entity);

            Matrix4 view = camera_transform->get_matrix().inversed();
            
            Matrix4 projection;
            if (active_camera->type == CameraType::Orthographic) {
                float half_height = active_camera->orthographic_size;
                float half_width = half_height * viewport.aspect_ratio();
                projection = Matrix4::orthographic(
                    -half_width, half_width,
                    -half_height, half_height,
                    active_camera->near_plane,
                    active_camera->far_plane
                );
            } else {
                projection = Matrix4::perspective(
                    to_radians(active_camera->fov),
                    viewport.aspect_ratio(),
                    active_camera->near_plane,
                    active_camera->far_plane
                );
            }

            camera_data.matrix = projection * view;
            break;
        }
        rd.camera = &camera_data;

        rd.mesh_instances.clear();
        rd.mesh_instances.reserve(last_instance_count);

        float far_plane_sq = active_camera
            ? active_camera->far_plane * active_camera->far_plane
            : std::numeric_limits<float>::max();

        for (auto entity : scene.get_registry().view<Transform, MeshRenderer>()) {
            Transform& transform = scene.get_component<Transform>(entity);
            MeshRenderer& mesh_renderer = scene.get_component<MeshRenderer>(entity);

            if (camera_transform) {
                Vector3 to_object = transform.position - camera_transform->position;
                float max_scale = std::max({ transform.scale.x, transform.scale.y, transform.scale.z });
                float radius = mesh_renderer.mesh->bounding_radius * max_scale;
                float reach = far_plane_sq + radius * radius;
                if (to_object.dot(to_object) > reach) continue;
            }

            if (!(mesh_renderer.is_static && mesh_renderer.cache_valid)) {
                mesh_renderer.cached_matrix = transform.get_matrix();
                mesh_renderer.cache_valid = true;
            }

            rd.mesh_instances.push_back(MeshInstance{
                mesh_renderer.mesh,
                mesh_renderer.cached_matrix,
                mesh_renderer.materials
            });
        }
        last_instance_count = rd.mesh_instances.size();

        rd.sprite_instances.clear();
        rd.sprite_instances.reserve(last_sprite_count);

        for (auto entity : scene.get_registry().view<Transform, SpriteRenderer>()) {
            Transform& transform = scene.get_component<Transform>(entity);
            SpriteRenderer& sprite_renderer = scene.get_component<SpriteRenderer>(entity);

            if (!sprite_renderer.sprite) continue;

            rd.sprite_instances.push_back(SpriteInstance{
                sprite_renderer.sprite,
                transform.get_matrix(),
                sprite_renderer.color,
                sprite_renderer.flip_x,
                sprite_renderer.flip_y,
                sprite_renderer.layer
            });
        }
        last_sprite_count = rd.sprite_instances.size();
    };

private:
    size_t last_instance_count = 0;
    size_t last_sprite_count = 0;
};

#if __REFLECT_GENERATED__
#include ".generated/render_system.generated.hxx"
#endif
