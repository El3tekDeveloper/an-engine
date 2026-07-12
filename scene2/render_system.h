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

struct [[export]] Camera : public Component {
    [[export, range(1, 179)]]
    float fov = 60.0f;
    [[export, range(0.001, 100)]]
    float near_plane = 0.1f;
    [[export, range(1, 10000)]]
    float far_plane = 100.0f;
};

struct [[export]] MeshRenderer : public Component {
    MeshRenderer() = default;
    explicit MeshRenderer(const Mesh& mesh, const std::vector<Material*> materials = {})
        : mesh(&mesh), materials(materials) {}

    [[export]] const Mesh* mesh = &Mesh::Cube;
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

class RenderSystem : public System {
public:
    virtual ~RenderSystem() = default;
    
    CameraData camera_data;

    void draw(Scene& scene, RenderData& rd) override {
        Transform* camera_transform = nullptr;
        Camera* active_camera = nullptr;

        for (auto entity : scene.get_registry().view<Transform, Camera>()) {
            active_camera = &scene.get_component<Camera>(entity);
            camera_transform = &scene.get_component<Transform>(entity);

            Matrix4 view = camera_transform->get_matrix().inversed();
            Matrix4 projection = Matrix4::perspective(
                to_radians(active_camera->fov),
                (float)camera_data.viewport.width / (float)camera_data.viewport.height,
                active_camera->near_plane,
                active_camera->far_plane
            );

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
    };

private:
    size_t last_instance_count = 0;
};

#if __REFLECT_GENERATED__
#include ".generated/render_system.generated.hxx"
#endif
