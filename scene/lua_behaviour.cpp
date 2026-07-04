#include "lua_behaviour.h"
#include "lua_bindings.h"
#include "scene/game_object.h"
#include "render/renderer.h"
#include "core/debug/logger.h"
#include "core/io/file_access.h"

LuaBehaviour::LuaBehaviour(std::string script_path)
    : script_path(std::move(script_path)) {}

void LuaBehaviour::load_script() {
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

    lua["game_object"] = std::ref(game_object());
    lua["transform"] = std::ref(game_object().transform);
    
    lua["self"] = lua.globals();

    if (!IO::exists(script_path)) {
        LOG_ERROR("LuaBehaviour: script not found '{}'", script_path);
        return;
    }

    sol::protected_function_result result = lua.safe_script_file(
        script_path, sol::script_pass_on_error
    );

    if (!result.valid()) {
        sol::error err = result;
        LOG_ERROR("LuaBehaviour: failed to load '{}': {}", script_path, err.what());
        return;
    }

    fn_awake  = lua["awake"];
    fn_start  = lua["start"];
    fn_update = lua["update"];
    fn_draw   = lua["draw"];

    loaded = true;
    LOG_INFO("LuaBehaviour: loaded '{}'", script_path);
}

void LuaBehaviour::reload() {
    load_script();
    if (loaded) call_safely(fn_awake, "awake");
}

void LuaBehaviour::call_safely(sol::protected_function& fn, const char* hook_name) {
    if (!fn.valid()) return;

    sol::protected_function_result result = fn();
    if (!result.valid()) {
        sol::error err = result;
        LOG_ERROR("LuaBehaviour [{}]: error in {}(): {}", script_path, hook_name, err.what());
    }
}

void LuaBehaviour::awake() {
    load_script();
    if (loaded) call_safely(fn_awake, "awake");
}

void LuaBehaviour::start() {
    if (loaded) call_safely(fn_start, "start");
}

void LuaBehaviour::update() {
    if (loaded) call_safely(fn_update, "update");
}

void LuaBehaviour::draw(RenderData& /*rd*/) {
    if (loaded) call_safely(fn_draw, "draw");
}
