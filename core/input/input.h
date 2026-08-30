#pragma once
#include "core/input/input_listener.h"
#include "core/os/keyboard.h"
#include <map>
#include <unordered_map>

class InputSystem {
public:
    InputSystem() = default;
    ~InputSystem() = default;

    static InputSystem& get_singleton() {
        static InputSystem instance;
        return instance;
    }
    
    void begin_frame();
    void poll_event(void* p_event);
    
    bool is_key_pressed(Key key) const;
    bool is_key_just_pressed(Key key) const;
    bool is_key_just_released(Key key) const;

    bool is_mouse_button_pressed(MouseButton button) const;
    bool is_mouse_button_just_pressed(MouseButton button) const;
    bool is_mouse_button_just_released(MouseButton button) const;

    const Vector2& get_mouse_position() const;
    const Vector2& get_mouse_delta() const;

    void add_listener(InputListener* listener);
    void remove_listener(InputListener* listener);

private:
    std::map<InputListener*, InputListener*> listener_map;

    std::unordered_map<Key, bool> key_states;
    std::unordered_map<Key, bool> previous_key_states;

    std::unordered_map<MouseButton, bool> mouse_states;
    std::unordered_map<MouseButton, bool> previous_mouse_states;

    Vector2 mouse_position;
    Vector2 mouse_delta;
};

static InputSystem& Input = InputSystem::get_singleton();
