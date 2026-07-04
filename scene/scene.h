#pragma once
#include "game_object.h"
#include "render/renderer.h"
#include "scene/camera_component.h"
#include <string>

class Scene {
public:
    Scene() = default;
    ~Scene() = default;
    
    GameObject& create_game_object(std::string name = "");
    GameObject& create_camera(std::string name = "");

    void initialize();
    void update();
    void draw(RenderData& render_data);
    
    void save(std::string path = "");
    void load(const std::string& path, BumpAllocator& allocator);
    void print_tree() const;

    GameObject* find(const std::string& name);
    void destroy(GameObject* go);

    std::size_t count() const { return game_objects.size(); }
    const std::vector<std::unique_ptr<GameObject>>& all() const {
        return game_objects;
    }

private:
    std::vector<std::unique_ptr<GameObject>> game_objects;
    
    Camera* resolve_camera();

    template <typename T>
    void for_each_active_behaviour(T&& fn) {
        for (auto& go : game_objects) {
            if (!go->is_active()) continue;

            for (Behaviour* mb : go->behaviours) {
                if (mb) fn(*mb);
            }
        }
    }
};
