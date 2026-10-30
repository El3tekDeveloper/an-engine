#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "core/os/time.h"
#include "core/os/hot_reload.h"
#include "render/renderer.h"
#include "scene.h"
#include "system.h"
#include "scene_values.h"

class SceneManager {
public:
    using Factory = std::function<std::unique_ptr<Scene>()>;
    using RegisterScenesFn = void (*)();

    static SceneManager& get_singleton() {
        static SceneManager instance;
        return instance;
    }

    template<typename T, typename... TArgs>
    void register_scene(const std::string& name, TArgs... args) {
        factories[name] = [=]() { return std::make_unique<T>(args...); };
        if (std::find(index_map.begin(), index_map.end(), name) == index_map.end()) {
            index_map.push_back(name);
        }
    }

    bool start(HotReloadDesc desc, const fs::path& assets_path);
    void stop() { hot_reload.reset(); }

    void load(const std::string& name, float delay = 0.0f);
    void load(size_t index, float delay = 0.0f);
    void load_file(const std::string& path, float delay = 0.0f);
    
    void reload() {
        if (!current_scene_name.empty()) pending = current_scene_name;
    }

    void refresh() {
        if (current_scene_name.empty()) return;
        pending = current_scene_name;
        keep_state = true;
    }

    void save(const std::string& path) {
        if (scene) save_scene(*scene, path, current_scene_name);
    }

    void save_all();

    void unload() {
        scene.reset();
        current_scene_name.clear();
    }

    void update() {
        if (hot_reload) hot_reload->update();
        if (!pending.empty()) {
            pending.timer -= Time.delta_time;
            if (pending.timer <= 0.0f) switch_to_pending();
        }
        if (scene) scene->update();
    }

    void draw(ViewPort& viewport, RenderData& render_data) {
        if (scene) scene->draw(viewport, render_data);
    }

    Scene* current() { return scene.get(); }
    const std::string& current_name() const { return current_scene_name; }

private:
    SceneManager() = default;
    ~SceneManager() = default;

    void switch_to_pending();

    static std::string scene_path(const std::string& name) {
        return "assets://scenes/" + name + ".ascn";
    }

    struct PendingScene {
        std::string name;
        std::string file;
        size_t index = static_cast<size_t>(-1);
        float timer = 0.0f;

        bool empty() const { return name.empty(); }
        void clear() { name.clear(); file.clear(); index = static_cast<size_t>(-1); }
        PendingScene& operator=(const std::string& n) { name = n; file.clear(); index = static_cast<size_t>(-1); return *this; }
        PendingScene& operator=(const int& i) { name = "NON-NAME"; index = i; return *this; }
    };

    std::unique_ptr<HotReload> hot_reload;
    std::unique_ptr<Scene> scene;

    std::unordered_map<std::string, Factory> factories;
    std::vector<std::string> index_map;

    size_t current_scene_index = static_cast<size_t>(-1);
    std::string current_scene_name;

    PendingScene pending;
    std::string restore_name;
    
    bool keep_state = false;
    SceneValues start_values;
    SceneValues live_values;
};

static SceneManager& SceneManager = SceneManager::get_singleton();
