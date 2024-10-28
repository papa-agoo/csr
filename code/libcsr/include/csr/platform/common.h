////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "hid.h"

#include <csr/core/math/vec2.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum platform_renderer_type
{
    PLATFORM_RENDERER_UNKNOWN,

    PLATFORM_RENDERER_OPENGL,
};

enum platform_backend_type
{
    PLATFORM_BACKEND_UNKNOWN,

    PLATFORM_BACKEND_GLFW3,
};

struct platform_display_info
{
    struct string name;

    struct vec2 resolution;
    struct vec2 physical_size;

    f32 content_scale;
};

struct platform_renderer_info
{
    struct string name;

    enum platform_renderer_type type;

    struct version version;
    struct string version_shading_lang;

    // FIXME capabilities / features / ...
    s32 sample_count;
    s32 sample_count_max;

    bool debug_context_enabled;
};

struct platform_backend_info
{
    struct string name;

    struct version version;
    enum platform_backend_type type;

    // FIXME hid / os / ...
};

struct platform_info
{
    struct platform_display_info display;

    struct platform_backend_info backend;
    struct platform_renderer_info renderer;
};

struct platform_callbacks
{
    void (*on_error)(s32 id, struct string message);

    void (*on_window_close)();
    void (*on_window_resize)(s32 width, s32 height);

    void (*on_mouse_move)(f64 x, f64 y);
    void (*on_mouse_wheel_change)(f64 x, f64 y);
    void (*on_mouse_button_change)(enum mouse_button button, bool is_down);

    void (*on_keyboard_key_change)(enum keyboard_key key, bool is_down);
};

struct platform_init_info
{
    struct string title;

    u32 width;
    u32 height;

    bool fullscreen;
    bool enable_vsync;

    struct {
        u32 sample_count;
        bool debug_context;
    } hint;
};
