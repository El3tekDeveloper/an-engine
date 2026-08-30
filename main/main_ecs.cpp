#include "core/debug/logger.h"
#include "core/input/input.h"
#include "core/memory/memory.h"
#include "core/os/time.h"
#include "core/os/application.h"
#include "core/os/window.h"
#include "render/renderer.h"
#include "resources/resource_manager.h"
#include "resources/lua/lua_script.h"
#include "scene2/systems/physics_system.h"
#include "scene2/systems/render_system.h"
#include "scene2/scene.h"
#include "scene2/game_object.h"
#include "scene2/systems/script_system.h"
#include <algorithm>
#include <glad/glad.h>

static RenderData render_data = {
    .clear_color = "#314D79"
};

static Scene scene;

static std::mt19937& rng() {
    static std::mt19937 gen{std::random_device{}()};
    return gen;
}

Material* get_random_material() {
    std::uniform_real_distribution<float> distr(0.01f, 1.0f);

    Material* mat = ResourceManager.create<Material>();
    mat->albedo_factor = Color(distr(rng()), distr(rng()), distr(rng()), 1);
    return mat;
}

Vector3 get_random_point_in_box(Vector3 center, Vector3 extents) {
    std::uniform_real_distribution<float> disX(center.x - extents.x, center.x + extents.x);
    std::uniform_real_distribution<float> disY(center.y - extents.y, center.y + extents.y);
    std::uniform_real_distribution<float> disZ(center.z - extents.z, center.z + extents.z);

    return Vector3{ disX(rng()), disY(rng()), disZ(rng()) };
}

void spawn_cube(const Vector3& position) {
    GameObject cube = scene.create_game_object();
    cube.add_component<MeshRenderer>(Mesh::Cube, std::vector<Material*>{get_random_material()});
    RigidBody& rb = cube.add_component<RigidBody>();
    rb.restitution = 0.2f;
    AABB& aabb = cube.add_component<AABB>();
    aabb.half_extents = Vector3::One * 0.5f;
    cube.transform().position = position;
}

int main() {
    BumpAllocator transient_storage(GB(4));

    Application app;
    if (!app.is_running()) {
        LOG_ERROR("Application failed to start");
        return -1;
    }
    Window* window = app.create_window(WindowDesc{
        .title = "ECS (an-engine)",
        .width = 1280,
        .height = 720
    });
    if (!window) {
        LOG_ERROR("Window creation failed");
        return -1;
    }

    ViewPort viewport;
    viewport.width = 640;
    viewport.height = 360;

    TextureRenderTarget main_target;
    main_target.init(0, 0, viewport.width, viewport.height);
    viewport.target = &main_target;

    window->on_close = [&app]() { app.quit(); };

    Renderer renderer;
    if (!renderer.init(&transient_storage)) {
        LOG_ERROR("Renderer init failed");
        return -1;
    }

    ResourceManager.reserve<Material>(256);

    scene.add_system(std::make_unique<ScriptSystem>());
    scene.add_system(std::make_unique<PhysicsSystem>());
    scene.add_system(std::make_unique<RenderSystem>());

    scene.initialize();

    Entity camera = scene.create_entity();
    scene.assign_component<Transform>(camera);
    scene.assign_component<Camera>(camera);

    Transform& camera_transform = scene.get_component<Transform>(camera);
    camera_transform.position = Vector3(0.0f, 15.0f, 40.0f);
    camera_transform.rotation = Quaternion::Euler(0.0f, 0.0f, -20.0f);

    GameObject ground = scene.create_game_object();
    ground.add_script<LuaScript>("assets://scripts/ground.lua");

    Vector3 box_center = Vector3(0.0f, 3, -1.0f);
    Vector3 box_extents = Vector3(22, 3, 22);

    float spawn_interval = 0.001f;
    float spawn_timer = 0.0f;
    int spawn_counter = 0;

    float camera_speed = 10.0f;
    float mouse_sensitivity = 0.5f;
    Vector2 camera_angle = Vector2::Zero;

    bool camera_control_enabled = true;

    while (app.is_running()) {
        app.poll_events();

        Time.update();
        scene.update();
        
        // Free Camera Movement
        if (camera_control_enabled) {
            float mouse_x = Input.get_mouse_delta().x * mouse_sensitivity;
            float mouse_y = Input.get_mouse_delta().y * mouse_sensitivity;

            camera_angle.x -= mouse_y;
            camera_angle.x = std::clamp(camera_angle.x, -89.0f, 89.0f);
            camera_angle.y -= mouse_x;

            camera_transform.rotation.euler.z = camera_angle.x; // Pitch
            camera_transform.rotation.euler.x = camera_angle.y; // Yaw

            Vector3 forward = camera_transform.rotation.rotate(Vector3::Forward);
            Vector3 right = camera_transform.rotation.rotate(Vector3::Right);
            Vector3 movement = Vector3::Zero;

            forward.y = 0.0f;
            right.y = 0.0f;
            if (forward.length_squared() > 0.0f) forward.normalize();
            if (right.length_squared() > 0.0f) right.normalize();

            if (Input.is_key_pressed(Key::S))
                movement += forward;
            else if (Input.is_key_pressed(Key::W))
                movement -= forward;
            if (Input.is_key_pressed(Key::D))
                movement += right;
            else if (Input.is_key_pressed(Key::A))
                movement -= right;
            if (Input.is_key_pressed(Key::E))
                movement.y += 1.0f;
            else if (Input.is_key_pressed(Key::Q))
                movement.y -= 1.0f;

            if (movement.length_squared() > 0.0f)
                movement.normalize();

            camera_transform.position += movement * camera_speed * Time.delta_time();
        }

        spawn_timer += Time.delta_time();
        if (spawn_timer >= spawn_interval) {
            spawn_timer = 0.0f;
            spawn_cube(get_random_point_in_box(box_center, box_extents));
            LOG_DEBUG("physics entities: {}", spawn_counter++);
        }

        scene.draw(viewport, render_data);
        renderer.render(viewport, render_data);

        window->swap_buffers();
    }

    ResourceManager.clear_all();
    renderer.destroy();
    return 0;
}
