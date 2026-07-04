#pragma once
#include "core/math/quaternion.h"
#include "scene/behaviour.h"
#include <sol/sol.hpp>
#include <string>

class [[export]] LuaBehaviour : public Behaviour {
public:
    explicit LuaBehaviour(std::string script_path);
    LuaBehaviour() = default;
    ~LuaBehaviour() override = default;
    
    void reload();

    [[export]] std::string script_path;

protected:
    void awake() override;
    void start() override;
    void update() override;

private:
    void draw(RenderData& rd) override;

    sol::state lua;

    sol::protected_function fn_awake;
    sol::protected_function fn_start;
    sol::protected_function fn_update;
    sol::protected_function fn_draw;

    bool loaded = false;

    void load_script();
    void call_safely(sol::protected_function& fn, const char* hook_name);
};

#if __REFLECT_GENERATED__
#include ".generated/lua_behaviour.generated.hxx"
#endif
