#pragma once
#include <sol/sol.hpp>
#include <string>
#include "scene.h"

class LuaScript : public Script {
public:
    explicit LuaScript(std::string path) : script_path(IO::resolve_path(path)) {}

    std::string script_path;

    void request_reload() { pending_reload = true; }

    void awake(Scene& scene, Entity entity) override;
    void start(Scene& scene, Entity entity) override;
    void update(Scene& scene, Entity entity) override;
    void draw(Scene& scene, Entity entity, RenderData& rd) override;

private:
    sol::state lua;
    sol::protected_function fn_awake;
    sol::protected_function fn_start;
    sol::protected_function fn_update;
    sol::protected_function fn_draw;

    bool loaded = false;
    bool pending_reload = false;

    void load_script(Scene& scene, Entity entity);
    void call_safely(sol::protected_function& fn, const char* hook_name);
};
