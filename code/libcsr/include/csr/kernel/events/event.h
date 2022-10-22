////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/platform/hid.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum event_type
{
    EVENT_NONE              = 0,

    EVENT_WINDOW_ERROR,
    EVENT_WINDOW_CLOSE,
    EVENT_WINDOW_RESIZE,

    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_WHEEL,
    EVENT_MOUSE_BUTTON_UP,
    EVENT_MOUSE_BUTTON_DOWN,

    EVENT_KBD_KEY_UP,
    EVENT_KBD_KEY_DOWN,

    EVENT_GAMEPAD_BUTTON_UP,
    EVENT_GAMEPAD_BUTTON_DOWN,
};

struct window_event
{
    // new window size
    u32 width;
    u32 height;

    // platform error
    result_e error_id;
    const char *error_msg;
};

struct mouse_event
{
    // x position
    s32 x;
    s32 x_delta;

    // y position
    s32 y;
    s32 y_delta;

    // wheel deltas
    f64 wx;
    f64 wy;

    enum mouse_button button;
};

struct keyboard_event
{
    enum keyboard_key key;
};

struct gamepad_event
{
    enum gamepad_button button;
};

struct event
{
    enum event_type type;

    union
    {
        struct window_event window;
        struct mouse_event mouse;
        struct keyboard_event keyboard;
        struct gamepad_event gamepad;
    };
};

#define make_event(...) \
    make_compound_literal(struct event, __VA_ARGS__)

////////////////////////////////////////////////////////////

struct hid_callbacks
{
    // keyboard
    void (*on_kbd_key_down)(struct keyboard_event *event);
    void (*on_kbd_key_up)(struct keyboard_event *event);

    // mouse
    void (*on_mouse_button_down)(struct mouse_event *event);
    void (*on_mouse_button_up)(struct mouse_event *event);

    void (*on_mouse_move)(struct mouse_event *event);
    void (*on_mouse_wheel_spin)(struct mouse_event *event);

    // gamepad
    void (*on_gamepad_button_down)(struct gamepad_event *event);
    void (*on_gamepad_button_up)(struct gamepad_event *event);
};
