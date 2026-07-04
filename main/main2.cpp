#include "core/debug/logger.h"
#include "core/memory/memory.h"
#include "core/time/time.h"
#include "render/renderer.h"
#include "resources/resource_manager.h"
#include "scene/game_object.h"
#include "scene/scene.h"

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

    scene.initialize();
    scene.load("assets/scene.scene", transient_storage);
    scene.print_tree();
    
    while (platform.is_running()) {
        platform.poll_events(&renderer);
        
        Time.update();
        scene.update();

        scene.draw(render_data);
        renderer.render(render_data);
        platform.swap_buffers();
    }
    
    ResourceManager.destroy_all();
    renderer.destroy();
    platform.shutdown();
    return 0;
}


