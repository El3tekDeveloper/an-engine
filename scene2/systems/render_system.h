#pragma once
#include "resources/style_box.h"
#include "scene2/ecs.h"
#include "scene2/system.h"
#include "scene2/scene.h"
#include "scene2/registry.h"
#include "scene2/components/transform.h"
#include "scene2/components/camera.h"
#include "scene2/components/sprite_renderer.h"
#include "scene2/components/mesh_renderer.h"
#include <vector>
#include "scene2/components/panel.h"

inline Matrix4 get_world_matrix(Scene& scene, Entity id) {
    Matrix4 local = scene.get_component<Transform>(id).get_local_matrix();
    Entity parent = scene.get_parent(id);
    if (!entity::is_valid(parent) || !scene.get_registry().has<Transform>(parent))
        return local;
    return get_world_matrix(scene, parent) * local;
}

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

            Matrix4 view = get_world_matrix(scene, entity).inversed();
            
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
                    Math::to_radians(active_camera->fov),
                    viewport.aspect_ratio(),
                    active_camera->near_plane,
                    active_camera->far_plane
                );
            }

            camera_data.matrix = projection * view;

            Camera::s_active_camera = active_camera;
            active_camera->inverse_view_projection = camera_data.matrix.inversed();
            active_camera->viewport = viewport;
            
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
                mesh_renderer.cached_matrix = get_world_matrix(scene, entity);
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
            SpriteRenderer& sprite_renderer = scene.get_component<SpriteRenderer>(entity);

            if (!sprite_renderer.sprite) continue;

            rd.sprite_instances.push_back(SpriteInstance{
                sprite_renderer.sprite,
                get_world_matrix(scene, entity),
                sprite_renderer.color,
                sprite_renderer.flip_x,
                sprite_renderer.flip_y,
                sprite_renderer.layer
            });
        }
        last_sprite_count = rd.sprite_instances.size();

        rd.panel_instances.clear();
        rd.panel_instances.reserve(last_panel_count);

        for (auto entity : scene.get_registry().view<Transform, Panel>()) {
            Panel& panel = scene.get_component<Panel>(entity);

            if (!panel.style_box) continue;

            rd.panel_instances.push_back(PanelInstance{
                panel.style_box,
                get_world_matrix(scene, entity)
            });
        }
        last_panel_count = rd.panel_instances.size();
    };

    size_t last_panel_count = 0;
private:
    size_t last_instance_count = 0;
    size_t last_sprite_count = 0;
};

