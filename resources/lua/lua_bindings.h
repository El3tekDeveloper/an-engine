#pragma once
#include "core/input/input.h"
#include "core/input/input_listener.h"
#include "core/math/color.h"
#include "core/math/vector3.h"
#include "core/math/quaternion.h"
#include "core/os/keyboard.h"
#include "core/os/time.h"
#include "resources/material.h"
#include "resources/mesh.h"
#include "resources/resource_manager.h"
#include "resources/sprite.h"
#include "sol/raii.hpp"
#include "sol/table.hpp"
#include "tools/reflector/type_registry.h"
#include "scene2/game_object.h"
#include "scene2/components/component.h"
#include "scene2/components/component_ops.h"
#include <sol/sol.hpp>
#include <magic_enum/magic_enum.hpp>

struct ArgBox {
    float f{}; int i{}; bool b{}; std::string s{};
    Vector3 v3{}; Quaternion q{}; Material* mat{};
};

static void* arg_to_ptr(ArgBox& box, sol::object v, const Type* t) {
    if (!t) return nullptr;
    auto tn = t->get_name();
    if (tn == "float")  { box.f = v.as<float>(); return &box.f; }
    if (tn == "int")    { box.i = v.as<int>(); return &box.i; }
    if (tn == "bool")   { box.b = v.as<bool>(); return &box.b; }
    if (tn == "std::string") { box.s = v.as<std::string>(); return &box.s; }
    if (tn == "Vector3")     { box.v3 = v.as<Vector3>(); return &box.v3; }
    if (tn == "Quaternion")  { box.q = v.as<Quaternion>(); return &box.q; }
    if (tn == "Material*")   { box.mat = v.as<Material*>(); return &box.mat; }
    return nullptr;
}

static sol::object ret_from_ptr(sol::state_view lua, void* p, const Type* t) {
    if (!t || !p) return sol::make_object(lua, sol::lua_nil);
    auto tn = t->get_name();
    if (tn == "float")  return sol::make_object(lua, *static_cast<float*>(p));
    if (tn == "int")    return sol::make_object(lua, *static_cast<int*>(p));
    if (tn == "bool")   return sol::make_object(lua, *static_cast<bool*>(p));
    if (tn == "std::string") return sol::make_object(lua, *static_cast<std::string*>(p));
    if (tn == "Vector3")     return sol::make_object(lua, *static_cast<Vector3*>(p));
    if (tn == "Quaternion")  return sol::make_object(lua, *static_cast<Quaternion*>(p));
    if (tn == "Material*")   return sol::make_object(lua, *static_cast<Material**>(p));
    return sol::make_object(lua, sol::lua_nil);
}

static sol::object field_get(sol::state_view lua, void* base, const Field& f) {
    if (!f.type) return sol::make_object(lua, sol::lua_nil);
    void* p = static_cast<char*>(base) + f.offset;
    auto tn = f.type->get_name();
    if (tn == "float") return sol::make_object(lua, *static_cast<float*>(p));
    if (tn == "int") return sol::make_object(lua, *static_cast<int*>(p));
    if (tn == "bool") return sol::make_object(lua, *static_cast<bool*>(p));
    if (tn == "std::string") return sol::make_object(lua, *static_cast<std::string*>(p));
    if (tn == "Vector3") return sol::make_object(lua, *static_cast<Vector3*>(p));
    if (tn == "Quaternion") return sol::make_object(lua, *static_cast<Quaternion*>(p));
    if (tn == "Mesh") return sol::make_object(lua, *static_cast<Mesh*>(p));
    if (tn == "Mesh *") return sol::make_object(lua, *static_cast<Mesh* const*>(p));
    if (tn == "Sprite *") return sol::make_object(lua, *static_cast<Sprite* const*>(p));
    if (tn == "Material") return sol::make_object(lua, *static_cast<Material*>(p));
    return sol::make_object(lua, sol::nil);
}

static void field_set(void* base, const Field& f, sol::object v) {
    if (!f.type) return;
    void* p = static_cast<char*>(base) + f.offset;
    auto tn = f.type->get_name();
    if (tn == "float") *static_cast<float*>(p) = v.as<float>();
    else if (tn == "int") *static_cast<int*>(p) = v.as<int>();
    else if (tn == "bool") *static_cast<bool*>(p) = v.as<bool>();
    else if (tn == "std::string") *static_cast<std::string*>(p) = v.as<std::string>();
    else if (tn == "Vector3") *static_cast<Vector3*>(p) = v.as<Vector3>();
    else if (tn == "Quaternion") *static_cast<Quaternion*>(p) = v.as<Quaternion>();
    else if (tn == "Mesh") *static_cast<Mesh*>(p) = v.as<Mesh>();
    else if (tn == "Mesh *") *static_cast<Mesh**>(p) = v.as<Mesh*>();
    else if (tn == "Sprite *") *static_cast<Sprite**>(p) = v.as<Sprite*>();
    else if (tn == "Material") *static_cast<Material*>(p) = v.as<Material>();
}

template <typename T>
void register_enum_automatically(sol::state& lua, const std::string& lua_name) {
    sol::table enum_table = lua.create_table();
    constexpr auto entries = magic_enum::enum_entries<T>();
    for (const auto& [value, name] : entries) {
        enum_table[std::string(name)] = value;
    }
    lua[lua_name] = enum_table;
}

static inline void register_lua_bindings(sol::state& lua, Scene& scene) {
    sol::table time_table = lua.create_table();
    time_table.set_function("delta_time", []() {
        return Time.delta_time();
    });
    time_table.set_function("unscaled_delta_time", []() {
        return Time.unscaled_delta_time();
    });
    time_table.set_function("time", []() {
        return Time.time();
    });
    time_table.set_function("real_time", []() {
        return Time.real_time();
    });
    time_table.set_function("frame_count", []() {
        return Time.frame_count();
    });

    time_table.set_function("get_time_scale", []() {
        return Time.time_scale;
    });
    time_table.set_function("set_time_scale", [](float value) {
        Time.time_scale = value;
    });
    lua["Time"] = time_table;

    sol::table resource_manager_table = lua.create_table();
    resource_manager_table.set_function("get_texture", [](const std::string& path) {
        return ResourceManager.load<Texture>(path);
    });
    resource_manager_table.set_function("create_material", []() {
        auto mat = ResourceManager.create<Material>();
        return mat;
    });
    lua["ResourceManager"] = resource_manager_table;

    register_enum_automatically<Keyboard::Key>(lua, "Key");
    register_enum_automatically<Mouse::MouseButton>(lua, "MouseButton");
    sol::table input_table = lua.create_table();
    input_table.set_function("is_key_pressed", [](Keyboard::Key key) {
        return Input.is_key_pressed(key);
    }); 
    input_table.set_function("is_key_just_pressed", [](Keyboard::Key key) {
        return Input.is_key_just_pressed(key);
    });
    input_table.set_function("is_key_just_released", [](Keyboard::Key key) {
        return Input.is_key_just_released(key);
    });
    input_table.set_function("is_mouse_button_pressed", [](Mouse::MouseButton btn) {
        return Input.is_mouse_button_pressed(btn);
    }); 
    input_table.set_function("is_mouse_button_just_pressed", [](Mouse::MouseButton btn) {
        return Input.is_mouse_button_just_pressed(btn);
    });
    input_table.set_function("is_mouse_button_just_released", [](Mouse::MouseButton btn) {
        return Input.is_mouse_button_just_released(btn);
    });
    input_table.set_function("get_mouse_position", []() {
        return Input.get_mouse_position();
    });
    input_table.set_function("get_mouse_delta", []() {
        return Input.get_mouse_delta();
    });
    lua["Input"] = input_table;

    sol::table scene_table = lua.create_table();
    scene_table.set_function("create_game_object", [&scene]() -> GameObject {
        return scene.create_game_object();
    });
    scene_table.set_function("destroy", [&scene](GameObject game_object) {
        return scene.destroy(game_object);
    });
    lua["Scene"] = scene_table;

    for (Type* type : get_types_by_kind(TypeKind::Class)) {
        TypeClass* tc = static_cast<TypeClass*>(type);
        if (!tc || !tc->create_instance) continue;
        lua[type->get_name()] = tc;
    }

    lua.new_usertype<Vector3>("Vector3",
        sol::constructors<Vector3(), Vector3(float, float, float)>(),
        "x", &Vector3::x,
        "y", &Vector3::y,
        "z", &Vector3::z,
        sol::meta_function::addition, [](const Vector3& a, const Vector3& b) { return a + b; },
        sol::meta_function::subtraction, [](const Vector3& a, const Vector3& b) { return a - b; },
        sol::meta_function::multiplication, [](const Vector3& a, float s) { return a * s; },
        sol::meta_function::to_string, [](const Vector3& v) {
            return std::format("[{}, {}, {}]", v.x, v.y, v.z);
        }
    );

    lua.new_usertype<Vector2>("Vector2",
        sol::constructors<Vector2(), Vector2(float, float)>(),
        "x", &Vector2::x,
        "y", &Vector2::y,
        sol::meta_function::addition, [](const Vector2& a, const Vector2& b) { return a + b; },
        sol::meta_function::subtraction, [](const Vector2& a, const Vector2& b) { return a - b; },
        sol::meta_function::multiplication, [](const Vector2& a, float s) { return a * s; },
        sol::meta_function::to_string, [](const Vector2& v) {
            return std::format("[{}, {}]", v.x, v.y);
        }
    );
    
    lua.new_usertype<Quaternion::EulerAngles>("Euler",

        "x", sol::property(
            [](Quaternion::EulerAngles& e) {
                return static_cast<float>(e.x);
            },
            [](Quaternion::EulerAngles& e, float v) {
                e.x = v;
            }),

        "y", sol::property(
            [](Quaternion::EulerAngles& e) {
                return static_cast<float>(e.y);
            },
            [](Quaternion::EulerAngles& e, float v) {
                e.y = v;
            }),

        "z", sol::property(
            [](Quaternion::EulerAngles& e) {
                return static_cast<float>(e.z);
            },
            [](Quaternion::EulerAngles& e, float v) {
                e.z = v;
            })
    );

    lua.new_usertype<Quaternion>("Quaternion",
        sol::constructors<Quaternion()>(),
        "x", &Quaternion::_x,
        "y", &Quaternion::_y,
        "z", &Quaternion::_z,
        "w", &Quaternion::_w,
        "euler", &Quaternion::euler,
        "Euler", sol::factories([](float x, float y, float z) {
            return Quaternion::Euler(x, y, z);
        }),
        sol::meta_function::to_string, [](const Quaternion& q) {
            return std::format(
                "[{}, {}, {}, {}]",
                q._x, q._y, q._z, q._w
            );
        }
    );

    lua.new_usertype<Color>("Color",
        sol::constructors<Color(float, float, float, float)>(),
        "r", &Color::r,
        "g", &Color::g,
        "b", &Color::b,
        "a", &Color::a
    );

    lua.new_usertype<Material>("Material",
        sol::constructors<Material()>(),

        "albedo_factor", &Material::albedo_factor,
        "emissive_factor", &Material::emissive_factor,
        "uv_scale", &Material::uv_scale,
        "uv_offset", &Material::uv_offset,
        "metallic_roughness", &Material::metallic_roughness,
        "flags", &Material::flags,

        "albedo_texture", &Material::albedo_texture,
        "normal_texture", &Material::normal_texture,
        "metalrough_texture", &Material::metalrough_texture,
        "emissive_texture", &Material::emissive_texture,

        "set_albedo", &Material::set_albedo,
        "set_normal", &Material::set_normal,
        "set_metalrough", &Material::set_metalrough,
        "set_emissive", &Material::set_emissive,

        "gpu_size", &Material::gpu_size
    );
    
    lua.new_usertype<SpriteRect>("SpriteRect",
        sol::constructors<SpriteRect(), SpriteRect(int, int, int, int)>(),

        "x", &SpriteRect::x,
        "y", &SpriteRect::y,
        "width", &SpriteRect::width,
        "height", &SpriteRect::height
    );

    lua.new_usertype<Sprite>("Sprite",
        sol::constructors<Sprite()>(),

        "load", &Sprite::load,
        "is_valid", &Sprite::is_valid,
        "get_size", &Sprite::get_size,
        "get_uv_rect", &Sprite::get_uv_rect,
        "texture", &Sprite::texture,
        "rect", &Sprite::rect,
        "pivot", &Sprite::pivot
    );

    lua.new_usertype<Transform>("Transform",
        "position", &Transform::position,
        "rotation", &Transform::rotation,
        "scale", &Transform::scale
    );

    lua.new_usertype<Mesh>("Mesh",
        "name", &Mesh::name
    );

    sol::table mesh_table = lua.create_table();
    mesh_table["Cube"] = &Mesh::Cube;
    mesh_table["Sphere"] = &Mesh::Sphere;
    mesh_table["Capsule"] = &Mesh::Capsule;
    lua["Mesh"] = mesh_table;

    lua.new_usertype<Component>("Component",
        "type_name", &Component::get_type_name,
        sol::meta_function::index, [&lua](Component& c, const std::string& field) -> sol::object {
            TypeClass* tc = static_cast<TypeClass*>(get_type_by_name(c.get_type_name()));

            if (const RefFunction* fn = tc->find_function(field.c_str())) {
                Component* self = &c;
                return sol::make_object(lua, sol::as_function(
                    [&lua, self, fn](sol::variadic_args va) -> sol::object {
                        std::vector<ArgBox> boxes(fn->parameters.size());
                        std::vector<void*> args(fn->parameters.size());

                        for (size_t idx = 0; idx < fn->parameters.size(); ++idx) {
                            if (idx >= va.size())
                            return sol::make_object(lua, sol::lua_nil);
                            args[idx] = arg_to_ptr(boxes[idx], va[idx], fn->parameters[idx].type);
                        }

                        ArgBox ret_box;
                        fn->invoke(self, args.data(), &ret_box);
                            return ret_from_ptr(lua, &ret_box, fn->return_value.type);
                    }
                ));
            }

            const Field* f = tc ? tc->find_field(field.c_str()) : nullptr;
            return f ? field_get(lua, &c, *f) : sol::make_object(lua, sol::lua_nil);
        },
        sol::meta_function::new_index, [](Component& c, const std::string& field, sol::object value) {
            TypeClass* tc = static_cast<TypeClass*>(get_type_by_name(c.get_type_name()));
            const Field* f = tc ? tc->find_field(field.c_str()) : nullptr;
            if (f) field_set(&c, *f, value);
        }
    );

    lua.new_usertype<GameObject>("GameObject",
        sol::constructors<GameObject()>(),
        "transform", sol::property([](GameObject& go) -> Transform& { return go.transform(); }),

        "add_component", [](GameObject& go, TypeClass* tc) -> Component* {
            if (!tc) {
                LOG_ERROR("add_component: called with a nil type (class not reflected?)");
                return nullptr;
            }
            const ComponentTypeOps* ops = find_component_ops(tc);
            if (!ops || !ops->add) {
                LOG_ERROR("add_component: '{}' is not registered as a component", tc->get_name());
                return nullptr;
            }
            return ops->add(*go.scene, go.id);
        },
        "get_component", [](GameObject& go, TypeClass* tc) -> Component* {
            if (!tc) {
                LOG_ERROR("get_component: called with a nil type (class not reflected?)");
                return nullptr;
            }
            const ComponentTypeOps* ops = find_component_ops(tc);
            if (!ops || !ops->has || !ops->get) {
                LOG_ERROR("get_component: '{}' is not registered as a component (missing Component base?)", tc->get_name());
                return nullptr;
            }
            if (!ops->has(*go.scene, go.id)) return nullptr;
            return ops->get(*go.scene, go.id);
        },
        "has_component", [](GameObject& go, TypeClass* tc) -> bool {
            if (!tc) {
                LOG_ERROR("has_component: called with a nil type (class not reflected?)");
                return false;
            }
            const ComponentTypeOps* ops = find_component_ops(tc);
            return ops && ops->has && ops->has(*go.scene, go.id);
        }
    );
}
