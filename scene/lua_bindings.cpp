#include "lua_bindings.h"
#include "core/math/vector3.h"
#include "core/math/quaternion.h"
#include "core/os/time.h"
#include "resources/material.h"
#include "resources/resource_manager.h"
#include "scene/transform_component.h"
#include "scene/game_object.h"
#include "resources/mesh.h"
#include "sol/table.hpp"
#include "tools/reflector/runtime/type_registry.h"

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
    else if (tn == "Material") *static_cast<Material*>(p) = v.as<Material>();
}

void register_lua_bindings(sol::state& lua) {
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
        return ResourceManager.get_texture(path);
    });
    resource_manager_table.set_function("create_material", []() {
        return ResourceManager.create_material();
    });
    lua["ResourceManager"] = resource_manager_table;

    lua.new_usertype<Vector3>("Vector3",
        sol::constructors<Vector3(), Vector3(float, float, float)>(),
        "x", &Vector3::x,
        "y", &Vector3::y,
        "z", &Vector3::z,
        sol::meta_function::addition, [](const Vector3& a, const Vector3& b) { return a + b; },
        sol::meta_function::subtraction, [](const Vector3& a, const Vector3& b) { return a - b; },
        sol::meta_function::multiplication, [](const Vector3& a, float s) { return a * s; },
        sol::meta_function::to_string, [](const Vector3& v) {
            return std::format("({}, {}, {})", v.x, v.y, v.z);
        }
    );

    lua.new_usertype<Quaternion::EulerAngles>("Euler",
        "x", &Quaternion::EulerAngles::x,
        "y", &Quaternion::EulerAngles::y,
        "z", &Quaternion::EulerAngles::z,
        sol::meta_function::to_string, [](const Quaternion::EulerAngles& v) {
            return std::format(
                "({}, {}, {})",
                static_cast<float>(v.x),
                static_cast<float>(v.y),
                static_cast<float>(v.z)
            );
        }
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
                "({}, {}, {}, {})",
                q._x, q._y, q._z, q._w
            );
        }
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

    lua.new_usertype<Transform>("Transform",
        "position", &Transform::position,
        "rotation", &Transform::rotation,
        "scale", &Transform::scale
    );

    sol::table mesh_table = lua.create_table();
    mesh_table["Cube"]   = Mesh::Cube;
    mesh_table["Sphere"] = Mesh::Sphere;
    lua["Mesh"] = mesh_table;

    lua.new_usertype<TypeClass>("TypeClass",
        "name", sol::property([](TypeClass& tc) { return tc.get_name(); })
    );

    for (auto& [name, tc] : class_registry()) {
        if (!tc || !tc->create_instance) continue;
        lua[name] = tc;
    }

    lua.new_usertype<Component>("Component",
        "type_name", &Component::get_type_name,
        sol::meta_function::index, [&lua](Component& c, const std::string& field) -> sol::object {
            TypeClass* tc = get_class_by_name(c.get_type_name());
            
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
            TypeClass* tc = get_class_by_name(c.get_type_name());
            const Field* f = tc ? tc->find_field(field.c_str()) : nullptr;
            if (f) field_set(&c, *f, value);
        }
    );
    
    lua.new_usertype<GameObject>("GameObject",
        "name", sol::property(&GameObject::get_name, &GameObject::set_name),
        "active", sol::property(&GameObject::is_active, &GameObject::set_active),
        "transform", sol::property([](GameObject& go) -> Transform& { return go.transform; }),

        "add_component", [](GameObject& go, TypeClass* tc) -> Component* {
            static TypeClass* component_class = get_class_by_name("Component");
            if (!tc || !tc->create_instance || !tc->is_a(component_class)) return nullptr;
            return &go.add_component(tc);
        },
        "get_component", [](GameObject& go, TypeClass* tc) -> Component* {
            return go.find_component(tc->get_name());
        },
        "has_component", [](GameObject& go, TypeClass* tc) -> bool {
            return go.find_component(tc->get_name()) != nullptr;
        }
    );
}
