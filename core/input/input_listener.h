#pragma once
#include "core/math/vector2.h"
#include "core/os/keyboard.h"

namespace Mouse {

enum class MouseButton {
    Left,
    Middle,
    Right,
    X1,
    X2,
    Unknown
};

}

using namespace Keyboard;
using namespace Mouse;

class InputListener {
public:
    InputListener() = default;
    ~InputListener() = default;

protected:
    virtual void on_key_down(Key key) {};
    virtual void on_key_up(Key key) {};

    virtual void on_mouse_button_down(MouseButton button) {}
    virtual void on_mouse_button_up(MouseButton button) {}

    virtual void on_mouse_move(const Vector2& position, const Vector2& delta) {}
    virtual void on_mouse_wheel(float x, float y) {}

    friend class InputSystem;
};
