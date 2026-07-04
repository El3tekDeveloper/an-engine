#include "behaviour.h"
#include "game_object.h"

GameObject& Behaviour::game_object() {
    return *static_cast<GameObject*>(entity);
}

const GameObject& Behaviour::game_object() const {
    return *static_cast<const GameObject*>(entity);
}

void Behaviour::update_internal() {
    if (!started) {
        start();
        started = true;
    }
    update();
}
