#include "cgltf.h"
#include "core/debug/logger.h"
#include "core/memory/bump_allocator.h"
#include "core/memory/memory.h"
#include "core/os/platform.h"
#include "core/math/math_utils.h"
#include "render/renderer.h"
#include "resources/mesh.h"
#include "resources/material.h"
#include "resources/resource_manager.h"
#include "scene/transform_component.h"

struct TestModel {
    Matrix4 model = Matrix4::Identity;
    std::vector<MeshInstance> instances;
    
    MeshInstance* get_mesh_by_name(const std::string& name) {
        for (auto& instance : instances)
            if (instance.mesh.name == name)
                return &instance;
        return nullptr;
    }
};

static Matrix4 model_a = Matrix4::Identity;

static RenderData render_data = {
    .clear_color = "#3D5A8A"
};

int main() {
    BumpAllocator transient_storage(MB(50));

    Platform platform(Window{
        .title = "Cpp Kart",
        .width = 1280,
        .height = 720
    });

    Renderer renderer;
    if (!renderer.init(&platform.get_window(), &transient_storage)) {
        LOG_ERROR("Renderer init failed");
        return -1;
    }
    
    Transform camera_transform;
    camera_transform.position = Vector3(0.0f, 2.3f, 8.0f);
    camera_transform.rotation = Quaternion::Euler(0.0f, 0.0f, -20.0f);
    CameraData camera_data;
    
    Matrix4 view = camera_transform.get_matrix().inversed();

    Matrix4 projection = Matrix4::perspective(
        to_radians(60.0f),
        (float)camera_data.viewport.width / (float)camera_data.viewport.height,
        0.1f,
        100.0f
    );
    
    camera_data.matrix = projection * view;
    render_data.camera = &camera_data;

    model_a *= Matrix4::translation(-1.8f, 0.0f, 0.0f);
    
    Material* mat = ResourceManager.create_material();
    mat->set_albedo(ResourceManager.get_texture("/home/el3tek/Downloads/fetchimage.jpeg"));
    mat->albedo_factor = Color(0.5, 0.5, 0.5, 1);

    Material* mat1 = ResourceManager.create_material();
    mat1->set_albedo(ResourceManager.get_texture("assets://textures/car.png"));
    mat1->albedo_texture->set_filter(TextureFilter::Nearest);
    
    TestModel car;
    car.model *= Matrix4::scale(0.5);
    car.model *= Matrix4::translation(1.8f, -0.9f, 0.0f);
    {
        cgltf_options options = {};
        cgltf_data* data = nullptr;

        if (cgltf_parse_file(&options, "assets://models/Car.glb", &data) != cgltf_result_success) {
            printf("Failed to parse file\n");
            return -1;
        }
        if (cgltf_load_buffers(&options, data, "assets://models/Car.glb") != cgltf_result_success) {
            printf("Failed to load buffers\n");
            cgltf_free(data);
            return -1;
        }

        for (size_t ni = 0; ni < data->nodes_count; ++ni) {
            cgltf_node& node = data->nodes[ni];
            if (!node.mesh) continue;

            std::vector<Vertex>   vertices;
            std::vector<uint32_t> indices;
            std::vector<SubMesh>  submeshes;

            for (size_t pi = 0; pi < node.mesh->primitives_count; ++pi) {
                cgltf_primitive& prim = node.mesh->primitives[pi];

                uint32_t index_offset = (uint32_t)indices.size();
                uint32_t vertex_base  = (uint32_t)vertices.size();

                cgltf_accessor* pos_acc  = nullptr;
                cgltf_accessor* norm_acc = nullptr;
                cgltf_accessor* uv_acc   = nullptr;

                for (size_t ai = 0; ai < prim.attributes_count; ++ai) {
                    auto& attr = prim.attributes[ai];
                    if (attr.type == cgltf_attribute_type_position) pos_acc  = attr.data;
                    if (attr.type == cgltf_attribute_type_normal)   norm_acc = attr.data;
                    if (attr.type == cgltf_attribute_type_texcoord) uv_acc   = attr.data;
                }

                for (size_t vi = 0; vi < pos_acc->count; ++vi) {
                    Vertex v;
                    float tmp[3];
                    if (pos_acc  && cgltf_accessor_read_float(pos_acc,  vi, tmp, 3)) v.position = Vector3(tmp[0], tmp[1], tmp[2]);
                    if (norm_acc && cgltf_accessor_read_float(norm_acc, vi, tmp, 3)) v.normal   = Vector3(tmp[0], tmp[1], tmp[2]);
                    if (uv_acc   && cgltf_accessor_read_float(uv_acc,   vi, tmp, 2)) v.uv       = Vector2(tmp[0], 1.0f - tmp[1]);
                    vertices.push_back(v);
                }

                uint32_t index_count = (uint32_t)prim.indices->count;
                for (size_t ii = 0; ii < index_count; ++ii)
                    indices.push_back(vertex_base + (uint32_t)cgltf_accessor_read_index(prim.indices, ii));

                submeshes.push_back({ index_offset, index_count, (uint32_t)pi });
            }
            
            float mat[16];
            cgltf_node_transform_world(&node, mat);

            car.instances.push_back(MeshInstance{
                Mesh(node.name ? node.name : "", std::move(vertices), std::move(indices), std::move(submeshes)),
                Matrix4(mat).transposed(),
                {mat1}
            });
        }

        cgltf_free(data);
    }
    
    MeshInstance* car_body = car.get_mesh_by_name("car");
    MeshInstance* front_left_wheel = car.get_mesh_by_name("front_left_wheel");
    MeshInstance* front_right_wheel = car.get_mesh_by_name("front_right_wheel");
    MeshInstance* back_left_wheel = car.get_mesh_by_name("back_left_wheel");
    MeshInstance* back_right_wheel = car.get_mesh_by_name("back_right_wheel");
    
    front_left_wheel->model *= Matrix4::rotation_z(to_radians(20));
    front_right_wheel->model *= Matrix4::rotation_z(to_radians(20));
    back_left_wheel->model *= Matrix4::rotation_z(to_radians(20));
    back_right_wheel->model *= Matrix4::rotation_z(to_radians(20));

    float wheels_rotate_speed = 2.0f;

    while (platform.is_running()) {
        platform.poll_events(&renderer);

        model_a *= Matrix4::rotation(to_radians(0.001f), 1.0f, 1.0f, 0.0f);
        car.model *= Matrix4::rotation_y(to_radians(0.001f));

        render_data.mesh_instances.push_back({ Mesh::Cube, model_a, {mat} });
        
        front_left_wheel->model *= Matrix4::rotation_x(to_radians(0.01f) * wheels_rotate_speed); 
        front_right_wheel->model *= Matrix4::rotation_x(to_radians(0.01f) * wheels_rotate_speed); 
        back_left_wheel->model *= Matrix4::rotation_x(to_radians(0.01f) * wheels_rotate_speed); 
        back_right_wheel->model *= Matrix4::rotation_x(to_radians(0.01f) * wheels_rotate_speed); 
        
        render_data.mesh_instances.push_back({ car_body->mesh, car.model * car_body->model, car_body->materials });
        render_data.mesh_instances.push_back({ front_left_wheel->mesh, car.model * front_left_wheel->model, front_left_wheel->materials });
        render_data.mesh_instances.push_back({ front_right_wheel->mesh, car.model * front_right_wheel->model, front_right_wheel->materials });
        render_data.mesh_instances.push_back({ back_left_wheel->mesh, car.model * back_left_wheel->model, back_left_wheel->materials });
        render_data.mesh_instances.push_back({ back_right_wheel->mesh, car.model * back_right_wheel->model, back_right_wheel->materials });

        renderer.render(render_data);

        platform.swap_buffers();
    }
    
    ResourceManager.destroy_all();
    renderer.destroy();
    platform.shutdown();
    return 0;
}
