#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

#include "core/debug/logger.h"
#include "core/input/input.h"
#include "core/io/path.h"
#include "core/math/matrix4.h"
#include "core/math/vector3.h"
#include "core/math/quaternion.h"
#include "core/memory/memory.h"
#include "core/os/keyboard.h"
#include "core/os/time.h"
#include "core/os/application.h"
#include "render/renderer.h"
#include "resources/resource_manager.h"
#include "scene2/components/mesh_renderer.h"
#include "scene2/components/panel.h"
#include "scene2/ecs.h"
#include "scene2/game_object.h"
#include "scene2/systems/physics_system.h"
#include "scene2/systems/render_system.h"
#include "scene2/scene.h"
#include "scene2/systems/script_system.h"
#include "scene2/system.h"

#include "cgltf.h"

#if __has_include("project.sln.hxx")
#include "project.sln.hxx"
#endif

static RenderData render_data = {
    .clear_color = "#314D79"
};

static Scene scene;

// Compute local node matrix using existing Matrix4 functions (T * R * S)
static Matrix4 get_node_transform(const cgltf_node* node) {
    if (node->has_matrix) {
        return Matrix4(node->matrix);
    }

    Matrix4 t = Matrix4::Identity;
    Matrix4 r = Matrix4::Identity;
    Matrix4 s = Matrix4::Identity;

    if (node->has_translation) {
        t = Matrix4::translation(node->translation[0], node->translation[1], node->translation[2]);
    }
    
    if (node->has_rotation) {
        // Convert quaternion (x, y, z, w) to angle-axis for standard Matrix4::rotation
        Quaternion q(node->rotation[0], node->rotation[1], node->rotation[2], node->rotation[3]);
        
        // Calculate rotation angle and normalized axis from Quaternion
        float angle = 2.0f * std::acos(q._w);
        float sin_half = std::sqrt(1.0f - q._w * q._w);

        if (sin_half > 0.0001f) {
            Vector3 axis(q._x / sin_half, q._y / sin_half, q._z / sin_half);
            r = Matrix4::rotation(angle, axis);
        }
    }
    
    if (node->has_scale) {
        s = Matrix4::scale(node->scale[0], node->scale[1], node->scale[2]);
    }

    // Combine transform components: T * R * S
    return t * r * s;
}

// Recursive helper to process glTF nodes and apply transform hierarchy
static void process_gltf_node(const cgltf_node* node, 
                              const Matrix4& parent_transform, 
                              std::vector<Vertex>& vertices, 
                              std::vector<uint32_t>& indices, 
                              std::vector<SubMesh>& submeshes) 
{
    Matrix4 current_transform = parent_transform * get_node_transform(node);

    if (node->mesh) {
        const cgltf_mesh* gltf_mesh = node->mesh;

        for (cgltf_size p = 0; p < gltf_mesh->primitives_count; ++p) {
            const cgltf_primitive& primitive = gltf_mesh->primitives[p];

            if (primitive.type != cgltf_primitive_type_triangles) {
                continue;
            }

            uint32_t base_vertex = static_cast<uint32_t>(vertices.size());
            uint32_t index_offset = static_cast<uint32_t>(indices.size());

            const cgltf_accessor* pos_acc = nullptr;
            const cgltf_accessor* norm_acc = nullptr;
            const cgltf_accessor* uv_acc = nullptr;

            for (cgltf_size a = 0; a < primitive.attributes_count; ++a) {
                const cgltf_attribute& attr = primitive.attributes[a];
                if (attr.type == cgltf_attribute_type_position) pos_acc = attr.data;
                if (attr.type == cgltf_attribute_type_normal)   norm_acc = attr.data;
                if (attr.type == cgltf_attribute_type_texcoord) uv_acc = attr.data;
            }

            if (!pos_acc) continue;

            cgltf_size vertex_count = pos_acc->count;
            for (cgltf_size v = 0; v < vertex_count; ++v) {
                Vertex vert{};

                // 1. Position: Transform by Node Matrix
                Vector3 raw_pos;
                cgltf_accessor_read_float(pos_acc, v, &raw_pos.x, 3);
                vert.position = current_transform.transform_point(raw_pos);

                // 2. Normal: Transform by Node Direction Matrix
                if (norm_acc) {
                    Vector3 raw_norm;
                    cgltf_accessor_read_float(norm_acc, v, &raw_norm.x, 3);
                    vert.normal = current_transform.transform_direction(raw_norm).normalized();
                } else {
                    vert.normal = Vector3(0.0f, 1.0f, 0.0f);
                }

                // 3. UVs: Invert Y-axis (1.0 - V) for standard graphics engines
                if (uv_acc) {
                    cgltf_accessor_read_float(uv_acc, v, &vert.uv.x, 2);
                    vert.uv.y = 1.0f - vert.uv.y; 
                } else {
                    vert.uv = Vector2(0.0f, 0.0f);
                }

                vertices.push_back(vert);
            }

            // Extract Indices
            uint32_t primitive_index_count = 0;
            if (primitive.indices) {
                primitive_index_count = static_cast<uint32_t>(primitive.indices->count);
                for (cgltf_size k = 0; k < primitive.indices->count; ++k) {
                    uint32_t index = static_cast<uint32_t>(cgltf_accessor_read_index(primitive.indices, k));
                    indices.push_back(base_vertex + index);
                }
            } else {
                primitive_index_count = static_cast<uint32_t>(vertex_count);
                for (uint32_t k = 0; k < primitive_index_count; ++k) {
                    indices.push_back(base_vertex + k);
                }
            }

            SubMesh submesh{};
            submesh.index_offset = index_offset;
            submesh.index_count = primitive_index_count;
            submesh.material_index = primitive.material ? static_cast<uint32_t>(p) : 0;
            submeshes.push_back(submesh);
        }
    }

    // Traverse child nodes recursively
    for (cgltf_size c = 0; c < node->children_count; ++c) {
        process_gltf_node(node->children[c], current_transform, vertices, indices, submeshes);
    }
}

bool is_hovered(const Vector2& point, Transform trans) {
    return (point.x >= trans.position.x && point.x <= (trans.position.x + trans.scale.x) &&
            point.y >= trans.position.y && point.y <= (trans.position.y + trans.scale.y));
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    BumpAllocator transient_storage(GB(4));

    Application app;
    if (!app.is_running()) {
        LOG_ERROR("Application failed to start");
        return -1;
    }
    Window* window = app.create_window(WindowDesc{
        .title = "GUI (an-engine)",
        .width = 1280,
        .height = 720
    });
    if (!window) {
        LOG_ERROR("Window creation failed");
        return -1;
    }
    window->on_close = [&app]() { app.quit(); };

    SwapchainRenderTarget swap_chain(window);
    ViewPort viewport;
    viewport.width  = window->get_width();
    viewport.height = window->get_height();
    viewport.target = &swap_chain;

    window->on_resize = [&viewport](int w, int h) {
        viewport.resize(w, h);
    };

    Renderer renderer;
    if (!renderer.init(&transient_storage)) {
        LOG_ERROR("Renderer init failed");
        return -1;
    }

    scene.add_system(std::make_unique<ScriptSystem>());
    scene.add_system(std::make_unique<PhysicsSystem>());
    scene.add_system(std::make_unique<RenderSystem>());

    scene.initialize();

    Material* mat1 = ResourceManager.create<Material>();
    mat1->set_albedo(ResourceManager.load<Texture>("assets://textures/car.png"));
    mat1->albedo_texture->set_filter(TextureFilter::Nearest);

    Entity camera = scene.create_entity();
    scene.assign_component<Transform>(camera);
    scene.assign_component<Camera>(camera);

    Transform& camera_transform = scene.get_component<Transform>(camera);
    camera_transform.position = Vector3(0.0f, 2.0f, 8.0f);
    scene.get_component<Camera>(camera);

    cgltf_options options = {};
    cgltf_data* data = nullptr;
    std::string file = IO::resolve_path("assets://models/Car.glb");
    const char* filepath = file.c_str();

    cgltf_result result = cgltf_parse_file(&options, filepath, &data);
    if (result != cgltf_result_success) {
        LOG_ERROR("Failed to parse glTF file: %s (Error code: %d)", filepath, static_cast<int>(result));
        return -1;
    }

    result = cgltf_load_buffers(&options, data, filepath);
    if (result != cgltf_result_success) {
        LOG_ERROR("Failed to load glTF buffers: %s", filepath);
        cgltf_free(data);
        return -1;
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SubMesh> submeshes;

    cgltf_scene* target_scene = data->scene ? data->scene : &data->scenes[0];
    for (cgltf_size i = 0; i < target_scene->nodes_count; ++i) {
        process_gltf_node(target_scene->nodes[i], Matrix4::Identity, vertices, indices, submeshes);
    }

    cgltf_free(data);

    Mesh car_mesh = Mesh(filepath, vertices, indices, submeshes);

    StyleBox stylebox;
    stylebox.load<StyleBox>("assets://stylebox_test.res");

    GameObject panel = scene.create_game_object();
    panel.add_component<Panel>(stylebox);
    panel.transform().position = Vector2(5, 5);
    panel.transform().scale = Vector2(100, 200);
    

    GameObject car = scene.create_game_object();
    car.add_component<MeshRenderer>(car_mesh).materials = {mat1};

    while (app.is_running()) {
        app.poll_events();

        Time.update();
        scene.update();

        if (Input.is_key_pressed(Key::W)) {
            car.transform().position.z += 30 * Time.delta_time();
        } else if (Input.is_key_pressed(Key::S)) {
            car.transform().position.z -= 30 * Time.delta_time();
        }

        if (Input.is_key_pressed(Key::D)) {
            car.transform().rotation.euler.x += 30 * Time.delta_time();
        } else if (Input.is_key_pressed(Key::A)) {
            car.transform().rotation.euler.x -= 30 * Time.delta_time();
        }
        
        window->make_current();
        scene.draw(viewport, render_data);
        renderer.render(viewport, render_data);
        window->swap_buffers();
    }
    
    ResourceManager.clear_all();
    renderer.destroy();
    app.destroy_all_windows();
    return 0;
}
