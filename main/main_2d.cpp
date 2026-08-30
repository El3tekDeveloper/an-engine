#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "core/debug/logger.h"
#include "core/math/random.h"
#include "core/math/vector3.h"
#include "core/memory/memory.h"
#include "core/os/time.h"
#include "core/os/application.h"
#include "render/renderer.h"
#include "resources/resource_manager.h"
#include "resources/sprite.h"
#include "resources/texture.h"
#include "scene2/components/component.h"
#include "scene2/ecs.h"
#include "scene2/game_object.h"
#include "resources/lua/lua_script.h"
#include "scene2/systems/physics_system.h"
#include "scene2/registry.h"
#include "scene2/systems/render_system.h"
#include "scene2/scene.h"
#include "scene2/systems/script_system.h"
#include "scene2/systems/system.h"

static RenderData render_data = {
    .clear_color = "#314D79"
};

static Scene scene;

constexpr float PIXELS_PER_UNIT = 100.0f;

constexpr float SCREEN_HALF_HEIGHT = 1.5f;
constexpr float SCREEN_HALF_WIDTH = 2.667f;

constexpr float PIPE_SCALE_X = 40.0f;
constexpr float PIPE_SCALE_Y = 200.0f;
constexpr float PIPE_SPEED = 1.2f;
constexpr float PIPE_SPAWN_EVERY = 1.6f;
constexpr float PIPE_GAP = 1.0f;
constexpr float PIPE_GAP_MIN_Y = -0.6f;
constexpr float PIPE_GAP_MAX_Y = 0.6f;

constexpr float SPAWN_X = SCREEN_HALF_WIDTH + 0.3f;
constexpr float DESPAWN_X = -SCREEN_HALF_WIDTH - 0.6f;

enum class GameState {
    Playing,
    GameOver
};

struct GameStateComponent : public Component {
    GameState state = GameState::Playing;
    int score = 0;
    float pipe_spawn_timer = 0.0f;
};

struct Player : public Component {
    Vector3 start_position = Vector3::Zero;
    int pixel_width = 64;
    int pixel_height = 64;
};

struct Pipe : public Component {
    bool is_top = false;
    bool scored = false;
    int pixel_width = 2;
    int pixel_height = 2;
    ecs::Entity pair_entity = ecs::entity::INVALID;
};

bool aabb_overlap(const Vector3& pos_a, const Vector3& scale_a, int pixel_w_a, int pixel_h_a,
                   const Vector3& pos_b, const Vector3& scale_b, int pixel_w_b, int pixel_h_b) {
    float half_w_a = (pixel_w_a / PIXELS_PER_UNIT * scale_a.x) * 0.5f;
    float half_h_a = (pixel_h_a / PIXELS_PER_UNIT * scale_a.y) * 0.5f;
    float half_w_b = (pixel_w_b / PIXELS_PER_UNIT * scale_b.x) * 0.5f;
    float half_h_b = (pixel_h_b / PIXELS_PER_UNIT * scale_b.y) * 0.5f;

    bool overlap_x = std::fabs(pos_a.x - pos_b.x) < (half_w_a + half_w_b);
    bool overlap_y = std::fabs(pos_a.y - pos_b.y) < (half_h_a + half_h_b);
    return overlap_x && overlap_y;
}

class PipeSystem : public System {
public:
    void awake(Scene& scene) override {
        pipe_sprite.load("assets://sprites/Fish.png");
        pipe_sprite.rect = SpriteRect{ .x = 32, .y = 32, .width = 1, .height = 1 };
    }

    void update(Scene& scene) override {
        ecs::Registry& registry = scene.get_registry();

        ecs::Entity game_state_entity = find_first<GameStateComponent>(registry);
        if (!ecs::entity::is_valid(game_state_entity)) return;
        GameStateComponent& game_state = registry.get<GameStateComponent>(game_state_entity);

        ecs::Entity player_entity = find_first<Player, Transform>(registry);
        if (!ecs::entity::is_valid(player_entity)) return;
        Player& player = registry.get<Player>(player_entity);
        Transform& player_transform = registry.get<Transform>(player_entity);

        if (game_state.state == GameState::Playing) {
            update_playing(scene, registry, game_state, player, player_transform);
        } else {
            reset_run(scene, registry, game_state, player, player_transform);
        }
    }

private:
    Sprite pipe_sprite;

    template<typename... Ts>
    ecs::Entity find_first(ecs::Registry& registry) {
        for (auto e : registry.view<Ts...>())
            return e;
        return ecs::entity::INVALID;
    }

    void update_playing(Scene& scene, ecs::Registry& registry, GameStateComponent& game_state,
                         Player& player, Transform& player_transform) {
        game_state.pipe_spawn_timer += Time.delta_time();
        if (game_state.pipe_spawn_timer >= PIPE_SPAWN_EVERY) {
            game_state.pipe_spawn_timer = 0.0f;
            spawn_pipe_pair(scene);
        }

        std::vector<ecs::Entity> to_destroy;
        bool hit_something = false;

        for (auto e : registry.view<Transform, Pipe>()) {
            Transform& pipe_transform = registry.get<Transform>(e);
            Pipe& pipe = registry.get<Pipe>(e);

            pipe_transform.position.x -= PIPE_SPEED * Time.delta_time();

            if (pipe.is_top && !pipe.scored && pipe_transform.position.x < player_transform.position.x) {
                pipe.scored = true;
                game_state.score++;
                LOG_INFO("Score: {}", game_state.score);
            }

            if (aabb_overlap(player_transform.position, player_transform.scale * 0.5f,
                              player.pixel_width, player.pixel_height,
                              pipe_transform.position, pipe_transform.scale,
                              pipe.pixel_width, pipe.pixel_height)) {
                hit_something = true;
            }

            if (pipe_transform.position.x < DESPAWN_X) {
                to_destroy.push_back(e);
            }
        }

        for (auto e : to_destroy) scene.destroy(e);

        if (hit_something ||
            player_transform.position.y > SCREEN_HALF_HEIGHT ||
            player_transform.position.y < -SCREEN_HALF_HEIGHT) {
            game_state.state = GameState::GameOver;
        }
    }

    void reset_run(Scene& scene, ecs::Registry& registry, GameStateComponent& game_state,
                   Player& player, Transform& player_transform) {
        player_transform.position = player.start_position;
        player_transform.rotation = Quaternion::Identity;

        game_state.pipe_spawn_timer = 0.0f;
        game_state.score = 0;

        std::vector<ecs::Entity> pipes_to_destroy;
        for (auto e : registry.view<Pipe>())
            pipes_to_destroy.push_back(e);
        for (auto e : pipes_to_destroy)
            scene.destroy(e);

        game_state.state = GameState::Playing;
    }

    ecs::Entity spawn_pipe(Scene& scene, const Vector3& position, bool is_top) {
        ecs::Entity e = scene.create_entity();

        Transform& transform = scene.assign_component<Transform>(e);
        transform.position = position;
        transform.scale = Vector3(PIPE_SCALE_X, PIPE_SCALE_Y, 0);

        scene.assign_component<SpriteRenderer>(e, pipe_sprite).flip_y = is_top;

        Pipe& pipe = scene.assign_component<Pipe>(e);
        pipe.is_top = is_top;
        pipe.scored = false;
        pipe.pixel_width = pipe_sprite.rect.width;
        pipe.pixel_height = pipe_sprite.rect.height;

        return e;
    }

    void spawn_pipe_pair(Scene& scene) {
        float gap_center_y = Random::rangef(PIPE_GAP_MIN_Y, PIPE_GAP_MAX_Y);
        float pipe_world_height = (static_cast<float>(pipe_sprite.rect.height) / PIXELS_PER_UNIT) * PIPE_SCALE_Y;

        Vector3 bottom_pos(SPAWN_X, gap_center_y - PIPE_GAP * 0.5f - pipe_world_height * 0.5f, 0);
        Vector3 top_pos(SPAWN_X, gap_center_y + PIPE_GAP * 0.5f + pipe_world_height * 0.5f, 0);

        ecs::Entity top = spawn_pipe(scene, top_pos, true);
        ecs::Entity bottom = spawn_pipe(scene, bottom_pos, false);

        scene.get_component<Pipe>(top).pair_entity = bottom;
        scene.get_component<Pipe>(bottom).pair_entity = top;
    }
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
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
    window->on_close = [&app]() { app.quit(); };

    SwapchainRenderTarget swap_chain(window);
    ViewPort viewport;
    viewport.width  = window->get_width();
    viewport.height = window->get_height();
    viewport.target = &swap_chain;

    window->on_resize = [&viewport](int w, int h) {
        viewport.resize(w, h);
    };

    Window* window2 = app.create_window(WindowDesc{
        .title = "non (an-engine)",
        .width = 1280,
        .height = 720
    });
    if (!window2) {
        LOG_ERROR("Window2 creation failed");
        return -1;
    }
    window2->on_close = [&app]() { app.quit(); };

    ViewPort viewport2;
    viewport2.width = 640;
    viewport2.height = 360;
    TextureRenderTarget main_target;
    main_target.init(0, 0, viewport2.width, viewport2.height);
    viewport2.target = &main_target;

    Renderer renderer;
    if (!renderer.init(&transient_storage)) {
        LOG_ERROR("Renderer init failed");
        return -1;
    }

    scene.add_system(std::make_unique<ScriptSystem>());
    scene.add_system(std::make_unique<PhysicsSystem>());
    scene.add_system(std::make_unique<PipeSystem>());
    scene.add_system(std::make_unique<RenderSystem>());

    scene.initialize();
    
    Entity camera = scene.create_entity();
    scene.assign_component<Transform>(camera);
    scene.assign_component<Camera>(camera);

    Transform& camera_transform = scene.get_component<Transform>(camera);
    camera_transform.position = Vector3(0.0f, 0.0f, 1.0f);
    Camera& camera_component = scene.get_component<Camera>(camera);
    camera_component.type = CameraType::Orthographic;
    camera_component.orthographic_size = 1.5f;

    Sprite fish_sprite;
    fish_sprite.load("assets://sprites/Fish.png");
    fish_sprite.texture->set_filter(TextureFilter::Nearest);
    fish_sprite.rect = SpriteRect{ .width = 64, .height = 64 };

    GameObject fish = scene.create_game_object();
    fish.add_script<LuaScript>("assets://scripts/fish.lua");

    Player& player = fish.add_component<Player>();
    player.start_position = fish.transform().position;
    player.pixel_width = fish_sprite.rect.width;
    player.pixel_height = fish_sprite.rect.height;

    Entity game_state_entity = scene.create_entity();
    scene.assign_component<GameStateComponent>(game_state_entity);

    while (app.is_running()) {
        app.poll_events();

        Time.update();
        scene.update();
        
        window->make_current();
        scene.draw(viewport, render_data);
        renderer.render(viewport, render_data);
        window->swap_buffers();
        
        window2->make_current();
        scene.draw(viewport2, render_data);
        renderer.render(viewport2, render_data);
        window2->swap_buffers();
    }
    
    scene.save("assets://scene.ascn");
    ResourceManager.clear_all();
    renderer.destroy();
    app.destroy_all_windows();
    return 0;
}
