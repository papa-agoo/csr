////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/platform/hid.h>

#include <GLFW/glfw3.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define map_value(left, right) [left] = right


////////////////////////////////////////////////////////////////////////////////
// mouse buttons
////////////////////////////////////////////////////////////////////////////////

#define NUM_MOUSE_BUTTONS 16

static enum mouse_button mouse_btns_csr[NUM_MOUSE_BUTTONS] = {
    map_value(MOUSE_BUTTON_1, GLFW_MOUSE_BUTTON_1),
    map_value(MOUSE_BUTTON_2, GLFW_MOUSE_BUTTON_2),
    map_value(MOUSE_BUTTON_3, GLFW_MOUSE_BUTTON_3),
    map_value(MOUSE_BUTTON_4, GLFW_MOUSE_BUTTON_4),
    map_value(MOUSE_BUTTON_5, GLFW_MOUSE_BUTTON_5),
    map_value(MOUSE_BUTTON_6, GLFW_MOUSE_BUTTON_6),
    map_value(MOUSE_BUTTON_7, GLFW_MOUSE_BUTTON_7),
    map_value(MOUSE_BUTTON_8, GLFW_MOUSE_BUTTON_8),

    map_value(MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_LEFT),
    map_value(MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_RIGHT),
    map_value(MOUSE_BUTTON_MIDDLE, GLFW_MOUSE_BUTTON_MIDDLE),
};

static s32 mouse_btns_glfw[NUM_MOUSE_BUTTONS] = {
    map_value(GLFW_MOUSE_BUTTON_1, MOUSE_BUTTON_1),
    map_value(GLFW_MOUSE_BUTTON_2, MOUSE_BUTTON_2),
    map_value(GLFW_MOUSE_BUTTON_3, MOUSE_BUTTON_3),
    map_value(GLFW_MOUSE_BUTTON_4, MOUSE_BUTTON_4),
    map_value(GLFW_MOUSE_BUTTON_5, MOUSE_BUTTON_5),
    map_value(GLFW_MOUSE_BUTTON_6, MOUSE_BUTTON_6),
    map_value(GLFW_MOUSE_BUTTON_7, MOUSE_BUTTON_7),
    map_value(GLFW_MOUSE_BUTTON_8, MOUSE_BUTTON_8),

    map_value(GLFW_MOUSE_BUTTON_LEFT, MOUSE_BUTTON_LEFT),
    map_value(GLFW_MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_RIGHT),
    map_value(GLFW_MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_MIDDLE),
};

enum mouse_button get_csr_mouse_button(s32 glfw_button)
{
    return mouse_btns_glfw[glfw_button];
}

s32 get_glfw_mouse_button(enum mouse_button csr_button)
{
    return mouse_btns_glfw[csr_button];
}


////////////////////////////////////////////////////////////////////////////////
// keyboard keys
////////////////////////////////////////////////////////////////////////////////

#define KEYMAP_SIZE 350

static enum keyboard_key keymap_csr[KEYMAP_SIZE] = {
    map_value(KBD_KEY_UNKNOWN, GLFW_KEY_UNKNOWN),

    map_value(KBD_KEY_SPACE, GLFW_KEY_SPACE),
    map_value(KBD_KEY_APOSTROPHE, GLFW_KEY_APOSTROPHE),
    map_value(KBD_KEY_COMMA, GLFW_KEY_COMMA),
    map_value(KBD_KEY_MINUS, GLFW_KEY_MINUS),
    map_value(KBD_KEY_PERIOD, GLFW_KEY_PERIOD),
    map_value(KBD_KEY_SLASH, GLFW_KEY_SLASH),

    map_value(KBD_KEY_0, GLFW_KEY_0),
    map_value(KBD_KEY_1, GLFW_KEY_1),
    map_value(KBD_KEY_2, GLFW_KEY_2),
    map_value(KBD_KEY_3, GLFW_KEY_3),
    map_value(KBD_KEY_4, GLFW_KEY_4),
    map_value(KBD_KEY_5, GLFW_KEY_5),
    map_value(KBD_KEY_6, GLFW_KEY_6),
    map_value(KBD_KEY_7, GLFW_KEY_7),
    map_value(KBD_KEY_8, GLFW_KEY_8),
    map_value(KBD_KEY_9, GLFW_KEY_9),

    map_value(KBD_KEY_SEMICOLON, GLFW_KEY_SEMICOLON),
    map_value(KBD_KEY_EQUAL, GLFW_KEY_EQUAL),

    map_value(KBD_KEY_A, GLFW_KEY_A),
    map_value(KBD_KEY_B, GLFW_KEY_B),
    map_value(KBD_KEY_C, GLFW_KEY_C),
    map_value(KBD_KEY_D, GLFW_KEY_D),
    map_value(KBD_KEY_E, GLFW_KEY_E),
    map_value(KBD_KEY_F, GLFW_KEY_F),
    map_value(KBD_KEY_G, GLFW_KEY_G),
    map_value(KBD_KEY_H, GLFW_KEY_H),
    map_value(KBD_KEY_I, GLFW_KEY_I),
    map_value(KBD_KEY_J, GLFW_KEY_J),
    map_value(KBD_KEY_K, GLFW_KEY_K),
    map_value(KBD_KEY_L, GLFW_KEY_L),
    map_value(KBD_KEY_M, GLFW_KEY_M),
    map_value(KBD_KEY_N, GLFW_KEY_N),
    map_value(KBD_KEY_O, GLFW_KEY_O),
    map_value(KBD_KEY_P, GLFW_KEY_P),
    map_value(KBD_KEY_Q, GLFW_KEY_Q),
    map_value(KBD_KEY_R, GLFW_KEY_R),
    map_value(KBD_KEY_S, GLFW_KEY_S),
    map_value(KBD_KEY_T, GLFW_KEY_T),
    map_value(KBD_KEY_U, GLFW_KEY_U),
    map_value(KBD_KEY_V, GLFW_KEY_V),
    map_value(KBD_KEY_W, GLFW_KEY_W),
    map_value(KBD_KEY_X, GLFW_KEY_X),
    map_value(KBD_KEY_Y, GLFW_KEY_Y),
    map_value(KBD_KEY_Z, GLFW_KEY_Z),

    map_value(KBD_KEY_LEFT_BRACKET, GLFW_KEY_LEFT_BRACKET),
    map_value(KBD_KEY_BACKSLASH, GLFW_KEY_BACKSLASH),
    map_value(KBD_KEY_RIGHT_BRACKET, GLFW_KEY_RIGHT_BRACKET),
    map_value(KBD_KEY_GRAVE_ACCENT, GLFW_KEY_GRAVE_ACCENT),
    map_value(KBD_KEY_WORLD_1, GLFW_KEY_WORLD_1),
    map_value(KBD_KEY_WORLD_2, GLFW_KEY_WORLD_2),
    map_value(KBD_KEY_ESCAPE, GLFW_KEY_ESCAPE),
    map_value(KBD_KEY_ENTER, GLFW_KEY_ENTER),
    map_value(KBD_KEY_TAB, GLFW_KEY_TAB),
    map_value(KBD_KEY_BACKSPACE, GLFW_KEY_BACKSPACE),
    map_value(KBD_KEY_INSERT, GLFW_KEY_INSERT),
    map_value(KBD_KEY_DELETE, GLFW_KEY_DELETE),
    map_value(KBD_KEY_RIGHT, GLFW_KEY_RIGHT),
    map_value(KBD_KEY_LEFT, GLFW_KEY_LEFT),
    map_value(KBD_KEY_DOWN, GLFW_KEY_DOWN),
    map_value(KBD_KEY_UP, GLFW_KEY_UP),
    map_value(KBD_KEY_PAGE_UP, GLFW_KEY_PAGE_UP),
    map_value(KBD_KEY_PAGE_DOWN, GLFW_KEY_PAGE_DOWN),
    map_value(KBD_KEY_HOME, GLFW_KEY_HOME),
    map_value(KBD_KEY_END, GLFW_KEY_END),
    map_value(KBD_KEY_CAPS_LOCK, GLFW_KEY_CAPS_LOCK),
    map_value(KBD_KEY_SCROLL_LOCK, GLFW_KEY_SCROLL_LOCK),
    map_value(KBD_KEY_NUM_LOCK, GLFW_KEY_NUM_LOCK),
    map_value(KBD_KEY_PRINT_SCREEN, GLFW_KEY_PRINT_SCREEN),
    map_value(KBD_KEY_PAUSE, GLFW_KEY_PAUSE),

    map_value(KBD_KEY_F1, GLFW_KEY_F1),
    map_value(KBD_KEY_F2, GLFW_KEY_F2),
    map_value(KBD_KEY_F3, GLFW_KEY_F3),
    map_value(KBD_KEY_F4, GLFW_KEY_F4),
    map_value(KBD_KEY_F5, GLFW_KEY_F5),
    map_value(KBD_KEY_F6, GLFW_KEY_F6),
    map_value(KBD_KEY_F7, GLFW_KEY_F7),
    map_value(KBD_KEY_F8, GLFW_KEY_F8),
    map_value(KBD_KEY_F9, GLFW_KEY_F9),
    map_value(KBD_KEY_F10, GLFW_KEY_F10),
    map_value(KBD_KEY_F11, GLFW_KEY_F11),
    map_value(KBD_KEY_F12, GLFW_KEY_F12),
    map_value(KBD_KEY_F13, GLFW_KEY_F13),
    map_value(KBD_KEY_F14, GLFW_KEY_F14),
    map_value(KBD_KEY_F15, GLFW_KEY_F15),
    map_value(KBD_KEY_F16, GLFW_KEY_F16),
    map_value(KBD_KEY_F17, GLFW_KEY_F17),
    map_value(KBD_KEY_F18, GLFW_KEY_F18),
    map_value(KBD_KEY_F19, GLFW_KEY_F19),
    map_value(KBD_KEY_F20, GLFW_KEY_F20),
    map_value(KBD_KEY_F21, GLFW_KEY_F21),
    map_value(KBD_KEY_F22, GLFW_KEY_F22),
    map_value(KBD_KEY_F23, GLFW_KEY_F23),
    map_value(KBD_KEY_F24, GLFW_KEY_F24),
    map_value(KBD_KEY_F25, GLFW_KEY_F25),

    map_value(KBD_KEY_KP_0, GLFW_KEY_KP_0),
    map_value(KBD_KEY_KP_1, GLFW_KEY_KP_1),
    map_value(KBD_KEY_KP_2, GLFW_KEY_KP_2),
    map_value(KBD_KEY_KP_3, GLFW_KEY_KP_3),
    map_value(KBD_KEY_KP_4, GLFW_KEY_KP_4),
    map_value(KBD_KEY_KP_5, GLFW_KEY_KP_5),
    map_value(KBD_KEY_KP_6, GLFW_KEY_KP_6),
    map_value(KBD_KEY_KP_7, GLFW_KEY_KP_7),
    map_value(KBD_KEY_KP_8, GLFW_KEY_KP_8),
    map_value(KBD_KEY_KP_9, GLFW_KEY_KP_9),
    map_value(KBD_KEY_KP_DECIMAL, GLFW_KEY_KP_DECIMAL),
    map_value(KBD_KEY_KP_DIVIDE, GLFW_KEY_KP_DIVIDE),
    map_value(KBD_KEY_KP_MULTIPLY, GLFW_KEY_KP_MULTIPLY),
    map_value(KBD_KEY_KP_SUBTRACT, GLFW_KEY_KP_SUBTRACT),
    map_value(KBD_KEY_KP_ADD, GLFW_KEY_KP_ADD),
    map_value(KBD_KEY_KP_ENTER, GLFW_KEY_KP_ENTER),
    map_value(KBD_KEY_KP_EQUAL, GLFW_KEY_KP_EQUAL),

    map_value(KBD_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_SHIFT),
    map_value(KBD_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_CONTROL),
    map_value(KBD_KEY_LEFT_ALT, GLFW_KEY_LEFT_ALT),
    map_value(KBD_KEY_LEFT_SUPER, GLFW_KEY_LEFT_SUPER),

    map_value(KBD_KEY_RIGHT_SHIFT, GLFW_KEY_RIGHT_SHIFT),
    map_value(KBD_KEY_RIGHT_CONTROL, GLFW_KEY_RIGHT_CONTROL),
    map_value(KBD_KEY_RIGHT_ALT, GLFW_KEY_RIGHT_ALT),
    map_value(KBD_KEY_RIGHT_SUPER, GLFW_KEY_RIGHT_SUPER),

    map_value(KBD_KEY_MENU, GLFW_KEY_MENU),
    map_value(KBD_KEY_LAST, GLFW_KEY_LAST),
};

static s32 keymap_glfw[KEYMAP_SIZE] = {
    map_value(0, KBD_KEY_UNKNOWN),

    map_value(GLFW_KEY_SPACE, KBD_KEY_SPACE),
    map_value(GLFW_KEY_APOSTROPHE, KBD_KEY_APOSTROPHE),
    map_value(GLFW_KEY_COMMA, KBD_KEY_COMMA),
    map_value(GLFW_KEY_MINUS, KBD_KEY_MINUS),
    map_value(GLFW_KEY_PERIOD, KBD_KEY_PERIOD),
    map_value(GLFW_KEY_SLASH, KBD_KEY_SLASH),

    map_value(GLFW_KEY_0, KBD_KEY_0),
    map_value(GLFW_KEY_1, KBD_KEY_1),
    map_value(GLFW_KEY_2, KBD_KEY_2),
    map_value(GLFW_KEY_3, KBD_KEY_3),
    map_value(GLFW_KEY_4, KBD_KEY_4),
    map_value(GLFW_KEY_5, KBD_KEY_5),
    map_value(GLFW_KEY_6, KBD_KEY_6),
    map_value(GLFW_KEY_7, KBD_KEY_7),
    map_value(GLFW_KEY_8, KBD_KEY_8),
    map_value(GLFW_KEY_9, KBD_KEY_9),

    map_value(GLFW_KEY_SEMICOLON, KBD_KEY_SEMICOLON),
    map_value(GLFW_KEY_EQUAL, KBD_KEY_EQUAL),

    map_value(GLFW_KEY_A, KBD_KEY_A),
    map_value(GLFW_KEY_B, KBD_KEY_B),
    map_value(GLFW_KEY_C, KBD_KEY_C),
    map_value(GLFW_KEY_D, KBD_KEY_D),
    map_value(GLFW_KEY_E, KBD_KEY_E),
    map_value(GLFW_KEY_F, KBD_KEY_F),
    map_value(GLFW_KEY_G, KBD_KEY_G),
    map_value(GLFW_KEY_H, KBD_KEY_H),
    map_value(GLFW_KEY_I, KBD_KEY_I),
    map_value(GLFW_KEY_J, KBD_KEY_J),
    map_value(GLFW_KEY_K, KBD_KEY_K),
    map_value(GLFW_KEY_L, KBD_KEY_L),
    map_value(GLFW_KEY_M, KBD_KEY_M),
    map_value(GLFW_KEY_N, KBD_KEY_N),
    map_value(GLFW_KEY_O, KBD_KEY_O),
    map_value(GLFW_KEY_P, KBD_KEY_P),
    map_value(GLFW_KEY_Q, KBD_KEY_Q),
    map_value(GLFW_KEY_R, KBD_KEY_R),
    map_value(GLFW_KEY_S, KBD_KEY_S),
    map_value(GLFW_KEY_T, KBD_KEY_T),
    map_value(GLFW_KEY_U, KBD_KEY_U),
    map_value(GLFW_KEY_V, KBD_KEY_V),
    map_value(GLFW_KEY_W, KBD_KEY_W),
    map_value(GLFW_KEY_X, KBD_KEY_X),
    map_value(GLFW_KEY_Y, KBD_KEY_Y),
    map_value(GLFW_KEY_Z, KBD_KEY_Z),

    map_value(GLFW_KEY_LEFT_BRACKET, KBD_KEY_LEFT_BRACKET),
    map_value(GLFW_KEY_BACKSLASH, KBD_KEY_BACKSLASH),
    map_value(GLFW_KEY_RIGHT_BRACKET, KBD_KEY_RIGHT_BRACKET),
    map_value(GLFW_KEY_GRAVE_ACCENT, KBD_KEY_GRAVE_ACCENT),
    map_value(GLFW_KEY_WORLD_1, KBD_KEY_WORLD_1),
    map_value(GLFW_KEY_WORLD_2, KBD_KEY_WORLD_2),
    map_value(GLFW_KEY_ESCAPE, KBD_KEY_ESCAPE),
    map_value(GLFW_KEY_ENTER, KBD_KEY_ENTER),
    map_value(GLFW_KEY_TAB, KBD_KEY_TAB),
    map_value(GLFW_KEY_BACKSPACE, KBD_KEY_BACKSPACE),
    map_value(GLFW_KEY_INSERT, KBD_KEY_INSERT),
    map_value(GLFW_KEY_DELETE, KBD_KEY_DELETE),
    map_value(GLFW_KEY_RIGHT, KBD_KEY_RIGHT),
    map_value(GLFW_KEY_LEFT, KBD_KEY_LEFT),
    map_value(GLFW_KEY_DOWN, KBD_KEY_DOWN),
    map_value(GLFW_KEY_UP, KBD_KEY_UP),
    map_value(GLFW_KEY_PAGE_UP, KBD_KEY_PAGE_UP),
    map_value(GLFW_KEY_PAGE_DOWN, KBD_KEY_PAGE_DOWN),
    map_value(GLFW_KEY_HOME, KBD_KEY_HOME),
    map_value(GLFW_KEY_END, KBD_KEY_END),
    map_value(GLFW_KEY_CAPS_LOCK, KBD_KEY_CAPS_LOCK),
    map_value(GLFW_KEY_SCROLL_LOCK, KBD_KEY_SCROLL_LOCK),
    map_value(GLFW_KEY_NUM_LOCK, KBD_KEY_NUM_LOCK),
    map_value(GLFW_KEY_PRINT_SCREEN, KBD_KEY_PRINT_SCREEN),
    map_value(GLFW_KEY_PAUSE, KBD_KEY_PAUSE),

    map_value(GLFW_KEY_F1, KBD_KEY_F1),
    map_value(GLFW_KEY_F2, KBD_KEY_F2),
    map_value(GLFW_KEY_F3, KBD_KEY_F3),
    map_value(GLFW_KEY_F4, KBD_KEY_F4),
    map_value(GLFW_KEY_F5, KBD_KEY_F5),
    map_value(GLFW_KEY_F6, KBD_KEY_F6),
    map_value(GLFW_KEY_F7, KBD_KEY_F7),
    map_value(GLFW_KEY_F8, KBD_KEY_F8),
    map_value(GLFW_KEY_F9, KBD_KEY_F9),
    map_value(GLFW_KEY_F10, KBD_KEY_F10),
    map_value(GLFW_KEY_F11, KBD_KEY_F11),
    map_value(GLFW_KEY_F12, KBD_KEY_F12),
    map_value(GLFW_KEY_F13, KBD_KEY_F13),
    map_value(GLFW_KEY_F14, KBD_KEY_F14),
    map_value(GLFW_KEY_F15, KBD_KEY_F15),
    map_value(GLFW_KEY_F16, KBD_KEY_F16),
    map_value(GLFW_KEY_F17, KBD_KEY_F17),
    map_value(GLFW_KEY_F18, KBD_KEY_F18),
    map_value(GLFW_KEY_F19, KBD_KEY_F19),
    map_value(GLFW_KEY_F20, KBD_KEY_F20),
    map_value(GLFW_KEY_F21, KBD_KEY_F21),
    map_value(GLFW_KEY_F22, KBD_KEY_F22),
    map_value(GLFW_KEY_F23, KBD_KEY_F23),
    map_value(GLFW_KEY_F24, KBD_KEY_F24),
    map_value(GLFW_KEY_F25, KBD_KEY_F25),

    map_value(GLFW_KEY_KP_0, KBD_KEY_KP_0),
    map_value(GLFW_KEY_KP_1, KBD_KEY_KP_1),
    map_value(GLFW_KEY_KP_2, KBD_KEY_KP_2),
    map_value(GLFW_KEY_KP_3, KBD_KEY_KP_3),
    map_value(GLFW_KEY_KP_4, KBD_KEY_KP_4),
    map_value(GLFW_KEY_KP_5, KBD_KEY_KP_5),
    map_value(GLFW_KEY_KP_6, KBD_KEY_KP_6),
    map_value(GLFW_KEY_KP_7, KBD_KEY_KP_7),
    map_value(GLFW_KEY_KP_8, KBD_KEY_KP_8),
    map_value(GLFW_KEY_KP_9, KBD_KEY_KP_9),
    map_value(GLFW_KEY_KP_DECIMAL, KBD_KEY_KP_DECIMAL),
    map_value(GLFW_KEY_KP_DIVIDE, KBD_KEY_KP_DIVIDE),
    map_value(GLFW_KEY_KP_MULTIPLY, KBD_KEY_KP_MULTIPLY),
    map_value(GLFW_KEY_KP_SUBTRACT, KBD_KEY_KP_SUBTRACT),
    map_value(GLFW_KEY_KP_ADD, KBD_KEY_KP_ADD),
    map_value(GLFW_KEY_KP_ENTER, KBD_KEY_KP_ENTER),
    map_value(GLFW_KEY_KP_EQUAL, KBD_KEY_KP_EQUAL),

    map_value(GLFW_KEY_LEFT_SHIFT, KBD_KEY_LEFT_SHIFT),
    map_value(GLFW_KEY_LEFT_CONTROL, KBD_KEY_LEFT_CONTROL),
    map_value(GLFW_KEY_LEFT_ALT, KBD_KEY_LEFT_ALT),
    map_value(GLFW_KEY_LEFT_SUPER, KBD_KEY_LEFT_SUPER),

    map_value(GLFW_KEY_RIGHT_SHIFT, KBD_KEY_RIGHT_SHIFT),
    map_value(GLFW_KEY_RIGHT_CONTROL, KBD_KEY_RIGHT_CONTROL),
    map_value(GLFW_KEY_RIGHT_ALT, KBD_KEY_RIGHT_ALT),
    map_value(GLFW_KEY_RIGHT_SUPER, KBD_KEY_RIGHT_SUPER),

    map_value(GLFW_KEY_MENU, KBD_KEY_MENU),
    map_value(GLFW_KEY_LAST, KBD_KEY_LAST),
};

enum keyboard_key get_csr_kbd_key(s32 glfw_key)
{
    if (glfw_key == GLFW_KEY_UNKNOWN)
        return KBD_KEY_UNKNOWN;

    return keymap_glfw[glfw_key];
}

s32 get_glfw_kbd_key(enum keyboard_key csr_key)
{
    if (csr_key == KBD_KEY_UNKNOWN)
        return GLFW_KEY_UNKNOWN;

    return keymap_csr[csr_key];
}
