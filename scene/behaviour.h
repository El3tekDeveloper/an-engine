#pragma once
#include "component.h"
#include "render/renderer.h"

class GameObject;

class [[export]] Behaviour : public Component {
public:
    virtual ~Behaviour() = default;

    GameObject& game_object();
    const GameObject& game_object() const;

protected:
    virtual void awake() {}
    virtual void start() {};
    virtual void update() {};

private:
    bool started = false;
    
    virtual void draw(RenderData& render_data) {};
    void update_internal();

    friend class Scene;
};

#if __REFLECT_GENERATED__
#include ".generated/behaviour.generated.hxx"
#endif
