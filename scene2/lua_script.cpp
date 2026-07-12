#include "lua_script.h"
#include "scene2/lua_bindings.h"
#include "core/debug/logger.h"
#include "core/io/file_access.h"
#include "scene2/render_system.h"

void LuaScript::load_script(Scene& scene, Entity entity) {
    loaded = false;

    lua = sol::state{};
    lua.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::math,
        sol::lib::table,
        sol::lib::string,
        sol::lib::os
    );
    register_lua_bindings(lua);

    lua["entity"] = entity;
    lua["game_object"] = GameObject{ entity, &scene };
    if (scene.get_registry().has<Transform>(entity)) {
        lua["transform"] = std::ref(scene.get_component<Transform>(entity));
    }
    lua["self"] = lua.globals();

    if (!IO::exists(script_path)) {
        LOG_ERROR("LuaScript: script not found '{}'", script_path);
        return;
    }

    sol::protected_function_result result = lua.safe_script_file(
        script_path, sol::script_pass_on_error
    );

    if (!result.valid()) {
        sol::error err = result;
        LOG_ERROR("LuaScript: failed to load '{}': {}", script_path, err.what());
        return;
    }

    fn_awake  = lua["awake"];
    fn_start  = lua["start"];
    fn_update = lua["update"];
    fn_draw   = lua["draw"];

    loaded = true;
    LOG_INFO("LuaScript: loaded '{}'", script_path);
}

void LuaScript::call_safely(sol::protected_function& fn, const char* hook_name) {
    if (!fn.valid()) return;

    sol::protected_function_result result = fn();
    if (!result.valid()) {
        sol::error err = result;
        LOG_ERROR("LuaScript [{}]: error in {}(): {}", script_path, hook_name, err.what());
    }
}

void LuaScript::awake(Scene& scene, Entity entity) {
    load_script(scene, entity);
    if (loaded) call_safely(fn_awake, "awake");
}

void LuaScript::start(Scene& scene, Entity entity) {
    if (loaded) call_safely(fn_start, "start");
}

void LuaScript::update(Scene& scene, Entity entity) {
    if (pending_reload) {
        pending_reload = false;
        load_script(scene, entity);
        if (loaded) call_safely(fn_awake, "awake");
    }
    if (loaded) call_safely(fn_update, "update");
}

void LuaScript::draw(Scene& scene, Entity entity, RenderData& /*rd*/) {
    if (loaded) call_safely(fn_draw, "draw");
}
