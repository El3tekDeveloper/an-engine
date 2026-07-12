#include "keyboard.h"
#include "core/utils/string_utils.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace Keyboard;

std::unordered_map<Key, std::string> keycodes = {
    {Key::Escape, "Escape"},
    {Key::Tab, "Tab"},
    {Key::Backtab, "Backtab"},
    {Key::Backspace, "Backspace"},
    {Key::Enter, "Enter"},
    {Key::KpEnter, "Keypad Enter"},
    {Key::Insert, "Insert"},
    {Key::Delete, "Delete"},
    {Key::Pause, "Pause"},
    {Key::Print, "Print Screen"},
    {Key::Sysreq, "SysRq"},
    {Key::Clear, "Clear"},
    {Key::Home, "Home"},
    {Key::End, "End"},
    {Key::Left, "Left"},
    {Key::Up, "Up"},
    {Key::Right, "Right"},
    {Key::Down, "Down"},
    {Key::PageUp, "Page Up"},
    {Key::PageDown, "Page Down"},
    {Key::Shift, "Shift"},
    {Key::Ctrl, "Ctrl"},
    {Key::Meta, "Meta"},
    {Key::CmdOrCtrl, "CmdOrCtrl"},
    {Key::Alt, "Alt"},
    {Key::CapsLock, "Caps Lock"},
    {Key::NumLock, "Num Lock"},
    {Key::ScrollLock, "Scroll Lock"},

    {Key::F1, "F1"},
    {Key::F2, "F2"},
    {Key::F3, "F3"},
    {Key::F4, "F4"},
    {Key::F5, "F5"},
    {Key::F6, "F6"},
    {Key::F7, "F7"},
    {Key::F8, "F8"},
    {Key::F9, "F9"},
    {Key::F10, "F10"},
    {Key::F11, "F11"},
    {Key::F12, "F12"},
    {Key::F13, "F13"},
    {Key::F14, "F14"},
    {Key::F15, "F15"},
    {Key::F16, "F16"},
    {Key::F17, "F17"},
    {Key::F18, "F18"},
    {Key::F19, "F19"},
    {Key::F20, "F20"},
    {Key::F21, "F21"},
    {Key::F22, "F22"},
    {Key::F23, "F23"},
    {Key::F24, "F24"},
    {Key::F25, "F25"},
    {Key::F26, "F26"},
    {Key::F27, "F27"},
    {Key::F28, "F28"},
    {Key::F29, "F29"},
    {Key::F30, "F30"},
    {Key::F31, "F31"},
    {Key::F32, "F32"},
    {Key::F33, "F33"},
    {Key::F34, "F34"},
    {Key::F35, "F35"},

    {Key::KpMultiply, "Keypad *"},
    {Key::KpDivide, "Keypad /"},
    {Key::KpSubtract, "Keypad -"},
    {Key::KpPeriod, "Keypad ."},
    {Key::KpAdd, "Keypad +"},
    {Key::Kp0, "Keypad 0"},
    {Key::Kp1, "Keypad 1"},
    {Key::Kp2, "Keypad 2"},
    {Key::Kp3, "Keypad 3"},
    {Key::Kp4, "Keypad 4"},
    {Key::Kp5, "Keypad 5"},
    {Key::Kp6, "Keypad 6"},
    {Key::Kp7, "Keypad 7"},
    {Key::Kp8, "Keypad 8"},
    {Key::Kp9, "Keypad 9"},

    {Key::Menu, "Menu"},
    {Key::Hyper, "Hyper"},
    {Key::Help, "Help"},
    {Key::Back, "Back"},
    {Key::Forward, "Forward"},
    {Key::Stop, "Stop"},
    {Key::Refresh, "Refresh"},
    {Key::VolumeDown, "Volume Down"},
    {Key::VolumeMute, "Volume Mute"},
    {Key::VolumeUp, "Volume Up"},

    {Key::MediaPlay, "Media Play"},
    {Key::MediaStop, "Media Stop"},
    {Key::MediaPrevious, "Media Previous"},
    {Key::MediaNext, "Media Next"},
    {Key::MediaRecord, "Media Record"},

    {Key::Homepage, "Homepage"},
    {Key::Favorites, "Favorites"},
    {Key::Search, "Search"},
    {Key::Standby, "Standby"},
    {Key::OpenUrl, "Open URL"},
    {Key::LaunchMail, "Launch Mail"},
    {Key::LaunchMedia, "Launch Media"},

    {Key::Launch0, "Launch 0"},
    {Key::Launch1, "Launch 1"},
    {Key::Launch2, "Launch 2"},
    {Key::Launch3, "Launch 3"},
    {Key::Launch4, "Launch 4"},
    {Key::Launch5, "Launch 5"},
    {Key::Launch6, "Launch 6"},
    {Key::Launch7, "Launch 7"},
    {Key::Launch8, "Launch 8"},
    {Key::Launch9, "Launch 9"},
    {Key::LaunchA, "Launch A"},
    {Key::LaunchB, "Launch B"},
    {Key::LaunchC, "Launch C"},
    {Key::LaunchD, "Launch D"},
    {Key::LaunchE, "Launch E"},
    {Key::LaunchF, "Launch F"},

    {Key::Globe, "Globe"},
    {Key::Keyboard, "Keyboard"},
    {Key::JisEisu, "JIS Eisu"},
    {Key::JisKana, "JIS Kana"},
    {Key::Unknown, "Unknown"},
};

static const std::unordered_set<Key> non_unicode_keys = {
    Key::Escape,
    Key::Tab,
    Key::Backtab,
    Key::Backspace,
    Key::Enter,
    Key::KpEnter,
    Key::Insert,
    Key::Delete,
    Key::Pause,
    Key::Print,
    Key::Sysreq,
    Key::Clear,

    Key::Home,
    Key::End,
    Key::Left,
    Key::Up,
    Key::Right,
    Key::Down,
    Key::PageUp,
    Key::PageDown,

    Key::Shift,
    Key::Ctrl,
    Key::Meta,
    Key::Alt,
    Key::CapsLock,
    Key::NumLock,
    Key::ScrollLock,

    Key::F1,
    Key::F2,
    Key::F3,
    Key::F4,
    Key::F5,
    Key::F6,
    Key::F7,
    Key::F8,
    Key::F9,
    Key::F10,
    Key::F11,
    Key::F12,
    Key::F13,
    Key::F14,
    Key::F15,
    Key::F16,
    Key::F17,
    Key::F18,
    Key::F19,
    Key::F20,
    Key::F21,
    Key::F22,
    Key::F23,
    Key::F24,
    Key::F25,
    Key::F26,
    Key::F27,
    Key::F28,
    Key::F29,
    Key::F30,
    Key::F31,
    Key::F32,
    Key::F33,
    Key::F34,
    Key::F35,

    Key::Menu,
    Key::Hyper,
    Key::Help,
    Key::Back,
    Key::Forward,
    Key::Stop,
    Key::Refresh,

    Key::VolumeDown,
    Key::VolumeMute,
    Key::VolumeUp,

    Key::MediaPlay,
    Key::MediaStop,
    Key::MediaPrevious,
    Key::MediaNext,
    Key::MediaRecord,

    Key::Homepage,
    Key::Favorites,
    Key::Search,
    Key::Standby,
    Key::OpenUrl,
    Key::LaunchMail,
    Key::LaunchMedia,

    Key::Launch0,
    Key::Launch1,
    Key::Launch2,
    Key::Launch3,
    Key::Launch4,
    Key::Launch5,
    Key::Launch6,
    Key::Launch7,
    Key::Launch8,
    Key::Launch9,

    Key::LaunchA,
    Key::LaunchB,
    Key::LaunchC,
    Key::LaunchD,
    Key::LaunchE,
    Key::LaunchF,

    Key::Globe,
    Key::Keyboard,
    Key::JisEisu,
    Key::JisKana,
};

bool Keyboard::keycode_has_unicode(Key key) {
    return !non_unicode_keys.contains(key);
}

std::string Keyboard::keycode_at_string(Key code) {
    std::vector<std::string> keycode_string;
    if ((code & KeyModifierMask::CmdOrCtrl) != Key::None) {
        keycode_string.push_back(find_keycode_name(Key::Ctrl));
    } if ((code & KeyModifierMask::Ctrl) != Key::None) {
        keycode_string.push_back(find_keycode_name(Key::Ctrl));
    } if ((code & KeyModifierMask::Alt) != Key::None) {
        keycode_string.push_back(find_keycode_name(Key::Alt));
    } if ((code & KeyModifierMask::Shift) != Key::None) {
        keycode_string.push_back(find_keycode_name(Key::Shift));
    } if ((code & KeyModifierMask::CmdOrCtrl) != Key::None) {
        keycode_string.push_back(find_keycode_name(Key::Meta));
    } if ((code & KeyModifierMask::Meta) != Key::None) {
        keycode_string.push_back(find_keycode_name(Key::Meta));
    }
    
    Key base = code & KeyModifierMask::CodeMask;
    auto it = keycodes.find(base);
    if (it != keycodes.end())
        keycode_string.push_back(it->second);

    std::string result;
    for (size_t i = 0; i < keycode_string.size(); ++i) {
        if (i != 0)
            result += "+";
        result += keycode_string[i];
    }

    return result;
}

Key Keyboard::find_keycode(const std::string& keycode_string) {
    Key keycode = Key::None;
    std::vector<std::string> parts = split(keycode_string, '+');
    if (keycode_string.empty())
        return keycode;
    
    const std::string& last_part = parts[parts.size() - 1];
    for (const auto& [key, name] : keycodes) {
        if (nocasecmp_to(last_part, name) == 0) {
            keycode = key;
            break;
        }
    }
    
    for (int i = 0; i < parts.size() - 1; i++) {
        const std::string& part = parts[i];
        if (nocasecmp_to(part, find_keycode_name(Key::Shift)) == 0) {
            keycode |= KeyModifierMask::Shift;
        } else if (nocasecmp_to(part, find_keycode_name(Key::Ctrl)) == 0) {
            keycode |= KeyModifierMask::Ctrl;
        } else if (nocasecmp_to(part, find_keycode_name(Key::Meta)) == 0) {
            keycode |= KeyModifierMask::Meta;
        } else if (nocasecmp_to(part, find_keycode_name(Key::Alt)) == 0) {
            keycode |= KeyModifierMask::Alt;
        }
    }

    return keycode;
}

std::string Keyboard::find_keycode_name(Key code) {
    return keycodes[code];
}

inline std::string_view Keyboard::keycode_get_name_by_index(int index) {
    auto it = keycodes.begin();
    std::advance(it, index);
    return it->second;
}
