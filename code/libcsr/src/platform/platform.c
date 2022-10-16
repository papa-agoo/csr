////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/platform/platform_priv.h>
#include <csr/platform/platform_pimpl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct platform g_platform = {0};

struct platform* platform_ptr()
{
    return &g_platform;
}

////////////////////////////////////////////////////////////////////////////////

static void _on_error(s32 id, const char *message) {}

static void _on_window_close() {}
static void _on_window_resize(s32 width, s32 height) {}

static void _on_mouse_move(f64 x, f64 y) {}
static void _on_mouse_wheel_change(f64 x, f64 y) {}
static void _on_mouse_button_change(enum mouse_button button, bool is_down) {}

static void _on_keyboard_key_change(enum keyboard_key key, bool is_down) {}

////////////////////////////////////////////////////////////////////////////////

static void _init_platform_info(struct platform_info *info, struct platform_init_info *init_info)
{
    struct platform_renderer_info *renderer = &info->renderer;
    renderer->name = "unknown";

    struct platform_backend_info *backend = &info->backend;
    backend->name = "unknown";
}

static void _init_platform_callbacks(struct platform_callbacks *callbacks)
{
    callbacks->on_error = _on_error;

    callbacks->on_window_close = _on_window_close;
    callbacks->on_window_resize = _on_window_resize;

    callbacks->on_mouse_move = _on_mouse_move;
    callbacks->on_mouse_wheel_change = _on_mouse_wheel_change;
    callbacks->on_mouse_button_change = _on_mouse_button_change;

    callbacks->on_keyboard_key_change = _on_keyboard_key_change;
}

////////////////////////////////////////////////////////////////////////////////

result_e platform_init(struct platform_init_info *init_info)
{
    csr_assert(!platform_ptr()->is_initialized);

    check_ptr(init_info);
    check_ptr(init_info->title);

    check_expr(init_info->width > 0);
    check_expr(init_info->height > 0);

    ////////////////////////////////////////

    struct platform_info *info = &platform_ptr()->info;
    _init_platform_info(info, init_info);

    struct platform_callbacks *callbacks = &platform_ptr()->callbacks;
    _init_platform_callbacks(callbacks);

    ////////////////////////////////////////

    result_e result = platform_init_impl(init_info, callbacks);
    check_result(result, "could not init platform impl.");

    platform_get_info_impl(info);

    platform_ptr()->is_initialized = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void platform_quit()
{
    csr_assert(platform_ptr()->is_initialized);

    platform_quit_impl();

    platform_ptr()->is_initialized = false;
}

void platform_poll_events()
{
    csr_assert(platform_ptr()->is_initialized);

    platform_poll_events_impl();
}

void platform_swap_buffers()
{
    csr_assert(platform_ptr()->is_initialized);

    platform_swap_buffers_impl();
}

struct platform_callbacks* platform_get_callbacks()
{
    csr_assert(platform_ptr()->is_initialized);

    return &platform_ptr()->callbacks;
};

const struct platform_info* platform_get_info()
{
    csr_assert(platform_ptr()->is_initialized);

    return &platform_ptr()->info;
};

f64 platform_time_elapsed()
{
    csr_assert(platform_ptr()->is_initialized);

    return platform_time_elapsed_impl();
}

u64 platform_time_elapsed_ms()
{
    csr_assert(platform_ptr()->is_initialized);

    return platform_time_elapsed_impl() * 1000;
}

bool platform_hid_kbd_key_down(enum keyboard_key key)
{
    csr_assert(platform_ptr()->is_initialized);

    return platform_keyboard_key_down_impl(key);
}

bool platform_hid_mouse_button_down(enum mouse_button button)
{
    csr_assert(platform_ptr()->is_initialized);

    return platform_mouse_button_down_impl(button);
}

void platform_win_resize(u32 width, u32 height)
{
    csr_assert(platform_ptr()->is_initialized);

    platform_win_resize_impl(width, height);
}

void platform_win_set_vsync(bool state)
{
    csr_assert(platform_ptr()->is_initialized);

    platform_win_enable_vsync_impl(state);
}

void platform_win_set_fullscreen(bool state)
{
    csr_assert(platform_ptr()->is_initialized);

    platform_win_enable_fullscreen_impl(state);
}

void platform_win_set_mouse_capture(bool state)
{
    csr_assert(platform_ptr()->is_initialized);

    platform_win_enable_mouse_capture_impl(state);
}

#include <pwd.h> // FIXME move to impl.

const char* platform_get_user_home_dir()
{
    return getenv("HOME"); // FIXME move to impl.
}
