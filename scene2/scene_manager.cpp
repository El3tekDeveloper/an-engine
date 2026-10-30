#include "scene_manager.h"
#include "core/io/path.h"

bool SceneManager::start(HotReloadDesc desc, const fs::path& assets_path) {
    hot_reload = std::make_unique<HotReload>(std::move(desc));

    hot_reload->on_load = [this]() {
        auto register_scenes = hot_reload->symbol<RegisterScenesFn>("game_register_scenes");
        if (!register_scenes) return false;

        register_scenes();
        if (!restore_name.empty()) load(restore_name);
        return true;
    };
    hot_reload->on_unload = [this]() {
        if (!pending.empty()) restore_name = pending.name;
        else if (!current_scene_name.empty()) restore_name = current_scene_name;

        if (scene) {
            keep_state = restore_name == current_scene_name;
            if (keep_state) live_values = capture_scene_values(*scene);
        }

        pending.clear();
        unload();
        ResourceManager.clear<Texture>();
        factories.clear();
    };
    hot_reload->watch(assets_path, [this]() { refresh(); });

    if (!hot_reload->start()) {
        hot_reload.reset();
        return false;
    }
    return true;
}

void SceneManager::load(const std::string& name, float delay) {
    if (!factories.contains(name)) {
        LOG_ERROR("SceneManager: unknown scene '{}'", name);
        return;
    }
    pending = name;
    pending.timer = delay;

    auto it = std::find(index_map.begin(), index_map.end(), name);
    if (it != index_map.end()) {
        pending.index = std::distance(index_map.begin(), it);
    } else {
        pending.index = static_cast<size_t>(-1);
    }
}

void SceneManager::load(size_t index, float delay) {
    if (index >= index_map.size()) {
        LOG_ERROR("SceneManager: scene index {} out of range ({} scenes)", index, index_map.size());
        return;
    }
    load(index_map[index], delay);
}

void SceneManager::load_file(const std::string& path, float delay) {
    if (!IO::exists(path)) {
        LOG_ERROR("SceneManager: no scene file '{}'", path);
        return;
    }

    const std::string name = read_scene_name(path);
    if (!factories.contains(name)) {
        LOG_ERROR("SceneManager: '{}' needs unknown scene '{}'", path, name);
        return;
    }

    load(name, delay);
    pending.file = path;
    keep_state = false;
}

void SceneManager::save_all() {
    fs::create_directories(IO::resolve_path("assets://scenes"));

    for (const std::string& name : index_map) {
        const std::string path = scene_path(name);

        if (scene && name == current_scene_name) {
            save_scene(*scene, path, name);
            continue;
        }
        if (IO::exists(path)) continue;

        auto it = factories.find(name);
        if (it == factories.end()) continue;

        std::unique_ptr<Scene> fresh = it->second();
        fresh->initialize();
        save_scene(*fresh, path, name);
    }
}

void SceneManager::switch_to_pending()  {
    const std::string name = pending.name;
    const size_t index = pending.index;
    const std::string file = pending.file;
    pending.clear();
    
    auto it = factories.find(name);
    if (it == factories.end()) {
        LOG_ERROR("SceneManager: no factory for '{}'", name);
        keep_state = false;
        return;
    }

    const bool reloading = name == current_scene_name;
    if (scene && keep_state) live_values = capture_scene_values(*scene);
    scene.reset();
    if (reloading) ResourceManager.clear<Texture>();
    
    scene = it->second();
    scene->initialize();

    SceneValues start = capture_scene_values(*scene);
    if (keep_state) restore_scene_values(*scene, start_values, live_values);
    else if (!file.empty()) apply_scene_values(*scene, read_scene_values(file));
    start_values = std::move(start);
    keep_state = false;

    current_scene_name = name;
    current_scene_index = index;
    LOG_INFO("SceneManager: loaded scene '{}'", name);
}
