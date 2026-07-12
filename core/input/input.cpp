#include "input.h"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "core/math/vector3.h"
#include "core/os/keyboard.h"
#include <unordered_map>
#include <utility>

std::unordered_map<SDL_Keycode, Key> sdl2_key_map = {
    { SDLK_ESCAPE, Key::Escape },
    { SDLK_TAB, Key::Tab },
    { SDLK_BACKSPACE, Key::Backspace },
    { SDLK_RETURN, Key::Enter },
    { SDLK_KP_ENTER, Key::KpEnter },
    { SDLK_INSERT, Key::Insert },
    { SDLK_DELETE, Key::Delete },
    { SDLK_PAUSE, Key::Pause },
    { SDLK_PRINTSCREEN, Key::Print },
    { SDLK_SYSREQ, Key::Sysreq },
    { SDLK_CLEAR, Key::Clear },
    
    { SDLK_HOME, Key::Home },
    { SDLK_END, Key::End },
    { SDLK_LEFT, Key::Left },
    { SDLK_UP, Key::Up },
    { SDLK_RIGHT, Key::Right },
    { SDLK_DOWN, Key::Down },
    { SDLK_PAGEUP, Key::PageUp },
    { SDLK_PAGEDOWN, Key::PageDown },
    
    { SDLK_LSHIFT, Key::Shift },
    { SDLK_RSHIFT, Key::Shift },
    { SDLK_LCTRL, Key::Ctrl },
    { SDLK_RCTRL, Key::Ctrl },
    { SDLK_LGUI, Key::Meta },
    { SDLK_RGUI, Key::Meta },
    { SDLK_LALT, Key::Alt },
    { SDLK_RALT, Key::Alt },
    { SDLK_CAPSLOCK, Key::CapsLock },
    { SDLK_NUMLOCKCLEAR, Key::NumLock },
    { SDLK_SCROLLLOCK, Key::ScrollLock },
    
    { SDLK_F1, Key::F1 },
    { SDLK_F2, Key::F2 },
    { SDLK_F3, Key::F3 },
    { SDLK_F4, Key::F4 },
    { SDLK_F5, Key::F5 },
    { SDLK_F6, Key::F6 },
    { SDLK_F7, Key::F7 },
    { SDLK_F8, Key::F8 },
    { SDLK_F9, Key::F9 },
    { SDLK_F10, Key::F10 },
    { SDLK_F11, Key::F11 },
    { SDLK_F12, Key::F12 },
    { SDLK_F13, Key::F13 },
    { SDLK_F14, Key::F14 },
    { SDLK_F15, Key::F15 },
    { SDLK_F16, Key::F16 },
    { SDLK_F17, Key::F17 },
    { SDLK_F18, Key::F18 },
    { SDLK_F19, Key::F19 },
    { SDLK_F20, Key::F20 },
    { SDLK_F21, Key::F21 },
    { SDLK_F22, Key::F22 },
    { SDLK_F23, Key::F23 },
    { SDLK_F24, Key::F24 },

    { SDLK_KP_MULTIPLY, Key::KpMultiply },
    { SDLK_KP_DIVIDE, Key::KpDivide },
    { SDLK_KP_MINUS, Key::KpSubtract },
    { SDLK_KP_PERIOD, Key::KpPeriod },
    { SDLK_KP_PLUS, Key::KpAdd },
    { SDLK_KP_0, Key::Kp0 },
    { SDLK_KP_1, Key::Kp1 },
    { SDLK_KP_2, Key::Kp2 },
    { SDLK_KP_3, Key::Kp3 },
    { SDLK_KP_4, Key::Kp4 },
    { SDLK_KP_5, Key::Kp5 },
    { SDLK_KP_6, Key::Kp6 },
    { SDLK_KP_7, Key::Kp7 },
    { SDLK_KP_8, Key::Kp8 },
    { SDLK_KP_9, Key::Kp9 },

    { SDLK_APPLICATION, Key::Menu },
    { SDLK_HELP, Key::Help },
    { SDLK_AC_BACK, Key::Back },
    { SDLK_AC_FORWARD, Key::Forward },
    { SDLK_AC_STOP, Key::Stop },
    { SDLK_AC_REFRESH, Key::Refresh },
    { SDLK_VOLUMEDOWN, Key::VolumeDown },
    { SDLK_MUTE, Key::VolumeMute },
    { SDLK_VOLUMEUP, Key::VolumeUp },
    { SDLK_AUDIOPLAY, Key::MediaPlay },
    { SDLK_AUDIOSTOP, Key::MediaStop },
    { SDLK_AUDIOPREV, Key::MediaPrevious },
    { SDLK_AUDIONEXT, Key::MediaNext },
    { SDLK_AC_HOME, Key::Homepage },
    { SDLK_AC_BOOKMARKS, Key::Favorites },
    { SDLK_AC_SEARCH, Key::Search },
    { SDLK_SLEEP, Key::Standby },
    { SDLK_WWW, Key::OpenUrl },
    { SDLK_MAIL, Key::LaunchMail },
    { SDLK_MEDIASELECT, Key::LaunchMedia },

    { SDLK_SPACE, Key::Space },
    { SDLK_EXCLAIM, Key::Exclam },
    { SDLK_QUOTEDBL, Key::Quotedbl },
    { SDLK_HASH, Key::Numbersign },
    { SDLK_DOLLAR, Key::Dollar },
    { SDLK_PERCENT, Key::Percent },
    { SDLK_AMPERSAND, Key::Ampersand },
    { SDLK_QUOTE, Key::Apostrophe },
    { SDLK_LEFTPAREN, Key::ParenLeft },
    { SDLK_RIGHTPAREN, Key::ParenRight },
    { SDLK_ASTERISK, Key::Asterisk },
    { SDLK_PLUS, Key::Plus },
    { SDLK_COMMA, Key::Comma },
    { SDLK_MINUS, Key::Minus },
    { SDLK_PERIOD, Key::Period },
    { SDLK_SLASH, Key::Slash },
    { SDLK_0, Key::Key0 },
    { SDLK_1, Key::Key1 },
    { SDLK_2, Key::Key2 },
    { SDLK_3, Key::Key3 },
    { SDLK_4, Key::Key4 },
    { SDLK_5, Key::Key5 },
    { SDLK_6, Key::Key6 },
    { SDLK_7, Key::Key7 },
    { SDLK_8, Key::Key8 },
    { SDLK_9, Key::Key9 },
    { SDLK_COLON, Key::Colon },
    { SDLK_SEMICOLON, Key::Semicolon },
    { SDLK_LESS, Key::Less },
    { SDLK_EQUALS, Key::Equal },
    { SDLK_GREATER, Key::Greater },
    { SDLK_QUESTION, Key::Question },
    { SDLK_AT, Key::At },
    
    { SDLK_a, Key::A },
    { SDLK_b, Key::B },
    { SDLK_c, Key::C },
    { SDLK_d, Key::D },
    { SDLK_e, Key::E },
    { SDLK_f, Key::F },
    { SDLK_g, Key::G },
    { SDLK_h, Key::H },
    { SDLK_i, Key::I },
    { SDLK_j, Key::J },
    { SDLK_k, Key::K },
    { SDLK_l, Key::L },
    { SDLK_m, Key::M },
    { SDLK_n, Key::N },
    { SDLK_o, Key::O },
    { SDLK_p, Key::P },
    { SDLK_q, Key::Q },
    { SDLK_r, Key::R },
    { SDLK_s, Key::S },
    { SDLK_t, Key::T },
    { SDLK_u, Key::U },
    { SDLK_v, Key::V },
    { SDLK_w, Key::W },
    { SDLK_x, Key::X },
    { SDLK_y, Key::Y },
    { SDLK_z, Key::Z },

    { SDLK_LEFTBRACKET, Key::BracketLeft },
    { SDLK_BACKSLASH, Key::Backslash },
    { SDLK_RIGHTBRACKET, Key::BracketRight },
    { SDLK_CARET, Key::AsciiCircum },
    { SDLK_UNDERSCORE, Key::Underscore },
    { SDLK_BACKQUOTE, Key::QuoteLeft },
    
    { SDLK_UNKNOWN, Key::Unknown }
};

Key sdl_to_key(SDL_Keycode keycode) {
    auto it = sdl2_key_map.find(keycode);
    if (it != sdl2_key_map.end()) {
        return it->second;
    }
    return Key::Unknown;
}

MouseButton sdl_to_mouse_button(Uint8 button) {
    switch (button) {
        case SDL_BUTTON_LEFT:   return MouseButton::Left;
        case SDL_BUTTON_RIGHT:  return MouseButton::Right;
        case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
        case SDL_BUTTON_X1:     return MouseButton::X1;
        case SDL_BUTTON_X2:     return MouseButton::X2;
    }
    return MouseButton::Unknown;
}

void InputSystem::begin_frame() {
    previous_key_states = key_states;
    previous_mouse_states = mouse_states;

    mouse_delta = Vector3::Zero;
}

void InputSystem::poll_event(void* p_event) {
    SDL_Event* event = static_cast<SDL_Event*>(p_event);
    
    switch (event->type) {
        // Keyboard
        case SDL_KEYDOWN: {
            Key key = sdl_to_key(event->key.keysym.sym);
            key_states[key] = true;

            for (auto& [listener, _] : listener_map) {
                listener->on_key_down(key);
            }
            break;
        }
        case SDL_KEYUP: {
            Key key = sdl_to_key(event->key.keysym.sym);
            key_states[key] = false;

            for (auto& [listener, _] : listener_map) {
                listener->on_key_up(key);
            }
            break;
        }

        // Mouse
        case SDL_MOUSEBUTTONDOWN: {
            MouseButton button = sdl_to_mouse_button(event->button.button);
            mouse_states[button] = true;

            for (auto& [listener, _] : listener_map) {
                listener->on_mouse_button_down(button);
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            MouseButton button = sdl_to_mouse_button(event->button.button);
            mouse_states[button] = true;

            for (auto& [listener, _] : listener_map) {
                listener->on_mouse_button_up(button);
            }
            break;
        }
        case SDL_MOUSEMOTION: {
            mouse_position.x = event->motion.x;
            mouse_position.y = event->motion.y;

            mouse_delta.x = event->motion.xrel;
            mouse_delta.y = event->motion.yrel;

            for (auto& [listener, _] : listener_map) {
                listener->on_mouse_move(
                    mouse_position,
                    mouse_delta
                );
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            for (auto& [listener, _] : listener_map) {
                listener->on_mouse_wheel(
                    static_cast<float>(event->wheel.x),
                    static_cast<float>(event->wheel.y)
                );
            }
            break;
        }
    }
}

void InputSystem::add_listener(InputListener* listener) {
    listener_map.insert(std::make_pair<InputListener*, InputListener*>(
        std::forward<InputListener*>(listener),
        std::forward<InputListener*>(listener)
    ));
}

void InputSystem::remove_listener(InputListener* listener) {
    std::map<InputListener*, InputListener*>::iterator it = listener_map.find(listener);
    if (it != listener_map.end()) {
        listener_map.erase(it);
    }
}

bool InputSystem::is_key_pressed(Key key) const {
    auto it = key_states.find(key);
    return it != key_states.end() && it->second;
}

bool InputSystem::is_key_just_pressed(Key key) const {
    bool current = false;
    bool previous = false;

    if (auto it = key_states.find(key); it != key_states.end())
        current = it->second;

    if (auto it = previous_key_states.find(key); it != previous_key_states.end())
        previous = it->second;

    return current && !previous;
}

bool InputSystem::is_key_just_released(Key key) const {
    bool current = false;
    bool previous = false;

    if (auto it = key_states.find(key); it != key_states.end())
        current = it->second;

    if (auto it = previous_key_states.find(key); it != previous_key_states.end())
        previous = it->second;

    return !current && previous;
}

bool InputSystem::is_mouse_button_pressed(MouseButton button) const {
    auto it = mouse_states.find(button);
    return it != mouse_states.end() && it->second;
}

bool InputSystem::is_mouse_button_just_pressed(MouseButton button) const {
    bool current = false;
    bool previous = false;

    if (auto it = mouse_states.find(button); it != mouse_states.end())
        current = it->second;

    if (auto it = previous_mouse_states.find(button); it != previous_mouse_states.end())
        previous = it->second;

    return current && !previous;
}

bool InputSystem::is_mouse_button_just_released(MouseButton button) const {
    bool current = false;
    bool previous = false;

    if (auto it = mouse_states.find(button); it != mouse_states.end())
        current = it->second;

    if (auto it = previous_mouse_states.find(button); it != previous_mouse_states.end())
        previous = it->second;

    return !current && previous;
}

const Vector3& InputSystem::get_mouse_position() const {
    return mouse_position;
}

const Vector3& InputSystem::get_mouse_delta() const {
    return mouse_delta;
}
