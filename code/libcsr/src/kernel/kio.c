////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/memory/arena_priv.h>

#include <csr/kernel/kio.h>
#include <csr/kernel/kernel_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// logging
////////////////////////////////////////////////////////////////////////////////
struct log_db* kio_log_get_db()
{
    return ksrv_get_log_db();
}

void kio_log_message(enum log_level_type level, string_cstr module, string_cstr fmt, ...)
{
    struct arena *arena = kio_mem_get_main_arena();

    va_list args;
    va_start(args, fmt);

    struct string message = string_create_vfmt(arena, fmt, args);

    va_end(args);

    ////////////////////////////////////////

    switch (level)
    {
        case LOG_LEVEL_INFO:
            clog_info("( %s ) %S", module, &message);
        break;

        case LOG_LEVEL_NOTICE:
            clog_notice("( %s ) %S", module, &message);
        break;

        case LOG_LEVEL_WARN:
            clog_warn("( %s ) %S", module, &message);
        break;

        case LOG_LEVEL_ERROR:
            clog_error("( %s ) %S", module, &message);
        break;

        default:
            clog_trace("( %s ) %S", module, &message);
    }

    ////////////////////////////////////////

    struct log_db *log_db = kio_log_get_db();

    if (log_db)
    {
        struct frame_stats *stats = &ksrv_core_ptr()->frame_stats;

        u32 current_frame_time = kio_time_elapsed_ms_rtc() - stats->runtime_ms;

        struct log_message log_entry = {0};
        log_entry.level = level;
        log_entry.frame = stats->num_frames;
        log_entry.frame_time_ms = current_frame_time;
        log_entry.message = message;
        log_entry.module = string_copy(arena, make_string_from_cstr(module));

        log_db_add_message(log_db, &log_entry);
    }
}


////////////////////////////////////////////////////////////////////////////////
// env vars
////////////////////////////////////////////////////////////////////////////////
bool kio_env_set_var(string_cstr name, string_cstr value)
{
    return env_vars_set_var(ksrv_get_env_vars(), name, value);
}

struct string kio_env_get_var(string_cstr name)
{
    return env_vars_get_var(ksrv_get_env_vars(), name);
}

string_cstr kio_env_get_var_cstr(string_cstr name)
{
    return env_vars_get_var_cstr(ksrv_get_env_vars(), name);
}

struct string kio_env_expand_str(string_cstr str)
{
    return env_vars_expand_str(ksrv_get_env_vars(), str);
}

string_cstr kio_env_expand_str_cstr(string_cstr str)
{
    return env_vars_expand_str_cstr(ksrv_get_env_vars(), str);
}


////////////////////////////////////////////////////////////////////////////////
// memory
////////////////////////////////////////////////////////////////////////////////
struct arena* kio_mem_get_main_arena()
{
    check_quiet(ksrv_core_ptr()->is_initialized);

    return ksrv_core_ptr()->allocator.arena_main;

error:
    return _arena_priv_ptr();
}

struct arena* kio_mem_get_frame_arena()
{
    check_expr(ksrv_core_ptr()->is_initialized);

    return ksrv_core_ptr()->allocator.arena_frame;

error:
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// time
////////////////////////////////////////////////////////////////////////////////
f64 kio_time_elapsed()
{
    check_expr(ksrv_core_ptr()->is_initialized);

    return clock_time_elapsed(ksrv_core_ptr()->clock);

error:
    return 0;
}

f64 kio_time_elapsed_delta()
{
    check_expr(ksrv_core_ptr()->is_initialized);

    return clock_time_elapsed_delta(ksrv_core_ptr()->clock);

error:
    return 0;
}

f64 kio_time_elapsed_rtc()
{
    if (!ksrv_core_ptr()->is_initialized) return 0;

    return platform_time_elapsed();
}

u64 kio_time_elapsed_ms_rtc()
{
    if (!ksrv_core_ptr()->is_initialized) return 0;

    return platform_time_elapsed() * 1000;
}


////////////////////////////////////////////////////////////////////////////////
// hid
////////////////////////////////////////////////////////////////////////////////
void kio_hid_enable_mouse_capture(bool enable)
{
    check_expr(ksrv_core_ptr()->is_initialized);

    platform_win_set_mouse_capture(enable);

error:
    return;
}

bool kio_hid_kbd_key_down(enum keyboard_key key)
{
    check_expr(ksrv_core_ptr()->is_initialized);

    return platform_hid_kbd_key_down(key);

error:
    return false;
}

bool kio_hid_mouse_button_down(enum mouse_button button)
{
    check_expr(ksrv_core_ptr()->is_initialized);

    return platform_hid_mouse_button_down(button);

error:
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// video
////////////////////////////////////////////////////////////////////////////////
void kio_video_enable_vsync(bool enable)
{
    check_expr(ksrv_core_ptr()->is_initialized);

    platform_win_set_vsync(enable);

error:
    return;
}

void kio_video_toggle_vsync()
{
    check_expr(ksrv_core_ptr()->is_initialized);

    bool vsync = ksrv_video_ptr()->conf->window.vsync ^= 1;

    platform_win_set_vsync(vsync);

error:
    return;
}

void kio_video_enable_fullscreen(bool enable)
{
    check_expr(ksrv_core_ptr()->is_initialized);

    platform_win_set_fullscreen(enable);

error:
    return;
}

void kio_video_toggle_fullscreen()
{
    check_expr(ksrv_core_ptr()->is_initialized);

    bool fullscreen = ksrv_video_ptr()->conf->window.fullscreen ^= 1;

    platform_win_set_fullscreen(fullscreen);

error:
    return;
}

struct vec2 kio_video_get_window_resolution()
{
    check_expr(ksrv_core_ptr()->is_initialized);
    check_expr(ksrv_video_ptr()->is_initialized);

    return ksrv_video_ptr()->conf->window.video_mode.resolution;

error:
    return make_vec2(0, 0);
}

void kio_video_set_window_resolution(struct vec2 resolution)
{
    check_expr(ksrv_core_ptr()->is_initialized);
    check_expr(ksrv_video_ptr()->is_initialized);

    struct video_mode *vm = &ksrv_video_ptr()->conf->window.video_mode;
    vm->resolution = resolution;

    if (vm->preset != VIDEO_MODE_PRESET_CUSTOM) {
        *vm = video_mode_create(resolution);
    }

    platform_win_resize((u32)resolution.w, (u32)resolution.h);

error:
    return;
}

struct vec2 kio_video_get_display_resolution()
{
    check_expr(ksrv_core_ptr()->is_initialized);

    return platform_get_info()->display.resolution;

error:
    return make_vec2(0, 0);
}

f32 kio_video_get_content_scale()
{
    check_expr(ksrv_core_ptr()->is_initialized);

    f32 content_scale = platform_get_info()->display.content_scale;
    check_expr(content_scale > 0);

    return content_scale;

error:
    return 1.0;
}

xgl_framebuffer kio_video_get_framebuffer()
{
    check_expr(ksrv_video_ptr()->is_initialized);

    return xgl_get_swapchain_framebuffer(ksrv_video_ptr()->swapchain);

error:
    return (xgl_framebuffer){0};
}
