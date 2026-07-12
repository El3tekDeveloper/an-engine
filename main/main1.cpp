#include "core/debug/logger.h"
#include "core/memory/memory.h"
#include "core/os/time.h"
#include "render/renderer.h"
#include "resources/mesh.h"
#include "resources/resource_manager.h"
#include "scene/game_object.h"
#include "scene/lua_behaviour.h"
#include "scene/scene.h"
#include "scene/mesh_renderer.h"

static RenderData render_data = {
    .clear_color = "#0b0f19"
};

static Scene scene;

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
    
    Material* earth_mat = ResourceManager.create<Material>();
    earth_mat->set_albedo(ResourceManager.load<Texture>("assets/textures/2k_earth_daymap.jpg"));
    earth_mat->set_normal(ResourceManager.load<Texture>("assets/textures/2k_earth_normal_map.jpg"));
    earth_mat->set_metalrough(ResourceManager.load<Texture>("assets/textures/2k_earth_specular_map.jpg"));

    Material* sun_mat = ResourceManager.create<Material>();
    sun_mat->set_albedo(ResourceManager.load<Texture>("assets/textures/2k_sun.jpg"));
    
    Material* moon_mat = ResourceManager.create<Material>();
    moon_mat->set_albedo(ResourceManager.load<Texture>("assets/textures/2k_moon.jpg"));   
 
    GameObject& camera = scene.create_camera("Main Camera");
    camera.transform.position = Vector3(0.0f, 2.3f, 8.0f);
    camera.transform.rotation = Quaternion::Euler(0.0f, 0.0f, -20.0f);

    GameObject& earth = scene.create_game_object("Earth");
    earth.add_component<MeshRenderer>(Mesh::Sphere, std::vector<Material*>{earth_mat});
    
    GameObject& sun = scene.create_game_object("Sun");
    sun.add_component<MeshRenderer>(Mesh::Sphere, std::vector<Material*>{sun_mat});
    sun.transform.scale = Vector3(2.5);

    GameObject& moon = scene.create_game_object("Moon");
    moon.add_component<MeshRenderer>(Mesh::Sphere, std::vector<Material*>{moon_mat});
    moon.transform.scale = Vector3(0.5);
    moon.transform.position = earth.transform.position;

    GameObject& player = scene.create_game_object("Player");
    player.add_component<LuaBehaviour>("assets/scripts/player.lua");

    scene.initialize();
    scene.print_tree();

    float speed = 2.0f;
    float angle = 0.0f;
    
    while (platform.is_running()) {
        platform.poll_events(&renderer);
        
        Time.update();
        scene.update();
        
        angle += speed * Time.delta_time();
        float x = cosf(angle);
        float z = sinf(angle);
        float y = sinf(x * z);

        earth.transform.position = Vector3(x * 4.0f, 0, z * 4.0f);
        earth.transform.rotation *= Quaternion::from_axis_angle({1, 1, 1}, 2.0f * Time.delta_time());

        moon.transform.position = earth.transform.position + Vector3(x * 1.0f, y * 1.0f, z * 1.0f);
        moon.transform.rotation *= Quaternion::from_axis_angle({1, 1, 1}, 2.0f * Time.delta_time());
        
        sun.transform.rotation *= Quaternion::from_axis_angle({1, 1, 1}, 0.8f * Time.delta_time());

        scene.draw(render_data);
        renderer.render(render_data);
        platform.swap_buffers();
    }
    
    ResourceManager.clear_all();
    renderer.destroy();
    platform.shutdown();
    scene.save("assets/scene.scene");
    return 0;
}

