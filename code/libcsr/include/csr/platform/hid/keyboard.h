////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum keyboard_key
{
    KBD_KEY_UNKNOWN,

    KBD_KEY_SPACE,
    KBD_KEY_APOSTROPHE,
    KBD_KEY_COMMA,
    KBD_KEY_MINUS,
    KBD_KEY_PERIOD,
    KBD_KEY_SLASH,

    KBD_KEY_0,
    KBD_KEY_1,
    KBD_KEY_2,
    KBD_KEY_3,
    KBD_KEY_4,
    KBD_KEY_5,
    KBD_KEY_6,
    KBD_KEY_7,
    KBD_KEY_8,
    KBD_KEY_9,

    KBD_KEY_SEMICOLON,
    KBD_KEY_EQUAL,

    KBD_KEY_A,
    KBD_KEY_B,
    KBD_KEY_C,
    KBD_KEY_D,
    KBD_KEY_E,
    KBD_KEY_F,
    KBD_KEY_G,
    KBD_KEY_H,
    KBD_KEY_I,
    KBD_KEY_J,
    KBD_KEY_K,
    KBD_KEY_L,
    KBD_KEY_M,
    KBD_KEY_N,
    KBD_KEY_O,
    KBD_KEY_P,
    KBD_KEY_Q,
    KBD_KEY_R,
    KBD_KEY_S,
    KBD_KEY_T,
    KBD_KEY_U,
    KBD_KEY_V,
    KBD_KEY_W,
    KBD_KEY_X,
    KBD_KEY_Y,
    KBD_KEY_Z,

    KBD_KEY_LEFT_BRACKET,
    KBD_KEY_BACKSLASH,
    KBD_KEY_RIGHT_BRACKET,
    KBD_KEY_GRAVE_ACCENT,
    KBD_KEY_WORLD_1,
    KBD_KEY_WORLD_2,
    KBD_KEY_ESCAPE,
    KBD_KEY_ENTER,
    KBD_KEY_TAB,
    KBD_KEY_BACKSPACE,
    KBD_KEY_INSERT,
    KBD_KEY_DELETE,
    KBD_KEY_RIGHT,
    KBD_KEY_LEFT,
    KBD_KEY_DOWN,
    KBD_KEY_UP,
    KBD_KEY_PAGE_UP,
    KBD_KEY_PAGE_DOWN,
    KBD_KEY_HOME,
    KBD_KEY_END,
    KBD_KEY_CAPS_LOCK,
    KBD_KEY_SCROLL_LOCK,
    KBD_KEY_NUM_LOCK,
    KBD_KEY_PRINT_SCREEN,
    KBD_KEY_PAUSE,

    KBD_KEY_F1,
    KBD_KEY_F2,
    KBD_KEY_F3,
    KBD_KEY_F4,
    KBD_KEY_F5,
    KBD_KEY_F6,
    KBD_KEY_F7,
    KBD_KEY_F8,
    KBD_KEY_F9,
    KBD_KEY_F10,
    KBD_KEY_F11,
    KBD_KEY_F12,
    KBD_KEY_F13,
    KBD_KEY_F14,
    KBD_KEY_F15,
    KBD_KEY_F16,
    KBD_KEY_F17,
    KBD_KEY_F18,
    KBD_KEY_F19,
    KBD_KEY_F20,
    KBD_KEY_F21,
    KBD_KEY_F22,
    KBD_KEY_F23,
    KBD_KEY_F24,
    KBD_KEY_F25,

    KBD_KEY_KP_0,
    KBD_KEY_KP_1,
    KBD_KEY_KP_2,
    KBD_KEY_KP_3,
    KBD_KEY_KP_4,
    KBD_KEY_KP_5,
    KBD_KEY_KP_6,
    KBD_KEY_KP_7,
    KBD_KEY_KP_8,
    KBD_KEY_KP_9,
    KBD_KEY_KP_DECIMAL,
    KBD_KEY_KP_DIVIDE,
    KBD_KEY_KP_MULTIPLY,
    KBD_KEY_KP_SUBTRACT,
    KBD_KEY_KP_ADD,
    KBD_KEY_KP_ENTER,
    KBD_KEY_KP_EQUAL,

    KBD_KEY_LEFT_SHIFT,
    KBD_KEY_LEFT_CONTROL,
    KBD_KEY_LEFT_ALT,
    KBD_KEY_LEFT_SUPER,

    KBD_KEY_RIGHT_SHIFT,
    KBD_KEY_RIGHT_CONTROL,
    KBD_KEY_RIGHT_ALT,
    KBD_KEY_RIGHT_SUPER,

    KBD_KEY_MENU,
    KBD_KEY_LAST = KBD_KEY_MENU
};

static const char *keyboard_key_cstr(enum keyboard_key key)
{
    switch (key)
    {
        case KBD_KEY_SPACE:             return "KBD_KEY_SPACE";
        case KBD_KEY_APOSTROPHE:        return "KBD_KEY_APOSTROPHE";
        case KBD_KEY_COMMA:             return "KBD_KEY_COMMA";
        case KBD_KEY_MINUS:             return "KBD_KEY_MINUS";
        case KBD_KEY_PERIOD:            return "KBD_KEY_PERIOD";
        case KBD_KEY_SLASH:             return "KBD_KEY_SLASH";

        case KBD_KEY_0:                 return "KBD_KEY_0";
        case KBD_KEY_1:                 return "KBD_KEY_1";
        case KBD_KEY_2:                 return "KBD_KEY_2";
        case KBD_KEY_3:                 return "KBD_KEY_3";
        case KBD_KEY_4:                 return "KBD_KEY_4";
        case KBD_KEY_5:                 return "KBD_KEY_5";
        case KBD_KEY_6:                 return "KBD_KEY_6";
        case KBD_KEY_7:                 return "KBD_KEY_7";
        case KBD_KEY_8:                 return "KBD_KEY_8";
        case KBD_KEY_9:                 return "KBD_KEY_9";

        case KBD_KEY_SEMICOLON:         return "KBD_KEY_SEMICOLON";
        case KBD_KEY_EQUAL:             return "KBD_KEY_EQUAL";

        case KBD_KEY_A:                 return "KBD_KEY_A";
        case KBD_KEY_B:                 return "KBD_KEY_B";
        case KBD_KEY_C:                 return "KBD_KEY_C";
        case KBD_KEY_D:                 return "KBD_KEY_D";
        case KBD_KEY_E:                 return "KBD_KEY_E";
        case KBD_KEY_F:                 return "KBD_KEY_F";
        case KBD_KEY_G:                 return "KBD_KEY_G";
        case KBD_KEY_H:                 return "KBD_KEY_G";
        case KBD_KEY_I:                 return "KBD_KEY_I";
        case KBD_KEY_J:                 return "KBD_KEY_J";
        case KBD_KEY_K:                 return "KBD_KEY_K";
        case KBD_KEY_L:                 return "KBD_KEY_L";
        case KBD_KEY_M:                 return "KBD_KEY_M";
        case KBD_KEY_N:                 return "KBD_KEY_N";
        case KBD_KEY_O:                 return "KBD_KEY_O";
        case KBD_KEY_P:                 return "KBD_KEY_P";
        case KBD_KEY_Q:                 return "KBD_KEY_Q";
        case KBD_KEY_R:                 return "KBD_KEY_R";
        case KBD_KEY_S:                 return "KBD_KEY_S";
        case KBD_KEY_T:                 return "KBD_KEY_T";
        case KBD_KEY_U:                 return "KBD_KEY_U";
        case KBD_KEY_V:                 return "KBD_KEY_V";
        case KBD_KEY_W:                 return "KBD_KEY_W";
        case KBD_KEY_X:                 return "KBD_KEY_X";
        case KBD_KEY_Y:                 return "KBD_KEY_Y";
        case KBD_KEY_Z:                 return "KBD_KEY_Z";

        case KBD_KEY_LEFT_BRACKET:      return "KBD_KEY_LEFT_BRACKET";
        case KBD_KEY_BACKSLASH:         return "KBD_KEY_BACKSLASH";
        case KBD_KEY_RIGHT_BRACKET:     return "KBD_KEY_RIGHT_BRACKET";
        case KBD_KEY_GRAVE_ACCENT:      return "KBD_KEY_GRAVE_ACCENT";
        case KBD_KEY_WORLD_1:           return "KBD_KEY_WORLD_1";
        case KBD_KEY_WORLD_2:           return "KBD_KEY_WORLD_2";
        case KBD_KEY_ESCAPE:            return "KBD_KEY_ESCAPE";
        case KBD_KEY_ENTER:             return "KBD_KEY_ENTER";
        case KBD_KEY_TAB:               return "KBD_KEY_TAB";
        case KBD_KEY_BACKSPACE:         return "KBD_KEY_BACKSPACE";
        case KBD_KEY_INSERT:            return "KBD_KEY_INSERT";
        case KBD_KEY_DELETE:            return "KBD_KEY_DELETE";
        case KBD_KEY_RIGHT:             return "KBD_KEY_RIGHT";
        case KBD_KEY_LEFT:              return "KBD_KEY_LEFT";
        case KBD_KEY_DOWN:              return "KBD_KEY_DOWN";
        case KBD_KEY_UP:                return "KBD_KEY_UP";
        case KBD_KEY_PAGE_UP:           return "KBD_KEY_PAGE_UP";
        case KBD_KEY_PAGE_DOWN:         return "KBD_KEY_PAGE_DOWN";
        case KBD_KEY_HOME:              return "KBD_KEY_HOME";
        case KBD_KEY_END:               return "KBD_KEY_END";
        case KBD_KEY_CAPS_LOCK:         return "KBD_KEY_CAPS_LOCK";
        case KBD_KEY_SCROLL_LOCK:       return "KBD_KEY_SCROLL_LOCK";
        case KBD_KEY_NUM_LOCK:          return "KBD_KEY_NUM_LOCK";
        case KBD_KEY_PRINT_SCREEN:      return "KBD_KEY_PRINT_SCREEN";
        case KBD_KEY_PAUSE:             return "KBD_KEY_PAUSE";

        case KBD_KEY_F1:                return "KBD_KEY_F1";
        case KBD_KEY_F2:                return "KBD_KEY_F2";
        case KBD_KEY_F3:                return "KBD_KEY_F3";
        case KBD_KEY_F4:                return "KBD_KEY_F4";
        case KBD_KEY_F5:                return "KBD_KEY_F5";
        case KBD_KEY_F6:                return "KBD_KEY_F6";
        case KBD_KEY_F7:                return "KBD_KEY_F7";
        case KBD_KEY_F8:                return "KBD_KEY_F8";
        case KBD_KEY_F9:                return "KBD_KEY_F9";
        case KBD_KEY_F10:               return "KBD_KEY_F10";
        case KBD_KEY_F11:               return "KBD_KEY_F11";
        case KBD_KEY_F12:               return "KBD_KEY_F12";
        case KBD_KEY_F13:               return "KBD_KEY_F13";
        case KBD_KEY_F14:               return "KBD_KEY_F14";
        case KBD_KEY_F15:               return "KBD_KEY_F15";
        case KBD_KEY_F16:               return "KBD_KEY_F16";
        case KBD_KEY_F17:               return "KBD_KEY_F17";
        case KBD_KEY_F18:               return "KBD_KEY_F18";
        case KBD_KEY_F19:               return "KBD_KEY_F19";
        case KBD_KEY_F20:               return "KBD_KEY_F20";
        case KBD_KEY_F21:               return "KBD_KEY_F21";
        case KBD_KEY_F22:               return "KBD_KEY_F22";
        case KBD_KEY_F23:               return "KBD_KEY_F23";
        case KBD_KEY_F24:               return "KBD_KEY_F24";
        case KBD_KEY_F25:               return "KBD_KEY_F25";

        case KBD_KEY_KP_0:              return "KBD_KEY_KP_0";
        case KBD_KEY_KP_1:              return "KBD_KEY_KP_1";
        case KBD_KEY_KP_2:              return "KBD_KEY_KP_2";
        case KBD_KEY_KP_3:              return "KBD_KEY_KP_3";
        case KBD_KEY_KP_4:              return "KBD_KEY_KP_4";
        case KBD_KEY_KP_5:              return "KBD_KEY_KP_5";
        case KBD_KEY_KP_6:              return "KBD_KEY_KP_6";
        case KBD_KEY_KP_7:              return "KBD_KEY_KP_7";
        case KBD_KEY_KP_8:              return "KBD_KEY_KP_8";
        case KBD_KEY_KP_9:              return "KBD_KEY_KP_9";
        case KBD_KEY_KP_DECIMAL:        return "KBD_KEY_KP_DECIMAL";
        case KBD_KEY_KP_DIVIDE:         return "KBD_KEY_KP_DIVIDE";
        case KBD_KEY_KP_MULTIPLY:       return "KBD_KEY_KP_MULTIPLY";
        case KBD_KEY_KP_SUBTRACT:       return "KBD_KEY_KP_SUBTRACT";
        case KBD_KEY_KP_ADD:            return "KBD_KEY_KP_ADD";
        case KBD_KEY_KP_ENTER:          return "KBD_KEY_KP_ENTER";
        case KBD_KEY_KP_EQUAL:          return "KBD_KEY_KP_EQUAL";

        case KBD_KEY_LEFT_SHIFT:        return "KBD_KEY_LEFT_SHIFT";
        case KBD_KEY_LEFT_CONTROL:      return "KBD_KEY_LEFT_CONTROL";
        case KBD_KEY_LEFT_ALT:          return "KBD_KEY_LEFT_ALT";
        case KBD_KEY_LEFT_SUPER:        return "KBD_KEY_LEFT_SUPER";

        case KBD_KEY_RIGHT_SHIFT:       return "KBD_KEY_RIGHT_SHIFT";
        case KBD_KEY_RIGHT_CONTROL:     return "KBD_KEY_RIGHT_CONTROL";
        case KBD_KEY_RIGHT_ALT:         return "KBD_KEY_RIGHT_ALT";
        case KBD_KEY_RIGHT_SUPER:       return "KBD_KEY_RIGHT_SUPER";

        case KBD_KEY_MENU:              return "KBD_KEY_MENU";

        default:                        return "KBD_KEY_UNKNOWN";
    }
}
