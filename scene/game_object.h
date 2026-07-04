#pragma once
#include "entity.h"
#include "behaviour.h"
#include "transform_component.h"
#include <string>

class Scene;

enum class Tag {
    Defult,
    Camera
};

class GameObject : public Entity {
public:
    explicit GameObject(std::string name)
        : name(std::move(name)) {}
    
    Tag tag = Tag::Defult;
    Transform& transform = add_component<Transform>();

    const std::string& get_name() const { return name; }
    void set_name(const std::string& new_name) { name = new_name; }

    bool is_active() const { return active; }
    void set_active(bool value)  { active = value; }

private:
    std::string name;
    bool active = true;
    
    std::vector<Behaviour*> behaviours;

    void on_component_added(Component* comp) override {
        if (auto* mb = dynamic_cast<Behaviour*>(comp)) {
            behaviours.push_back(mb);
        }
    }
    
    friend class Scene;
};
