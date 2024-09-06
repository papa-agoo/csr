////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/kernel/kio.h>
#include <csr/applet/aio.h>

#include <csr/applet/applet_mgr.h>
#include <csr/applet/applet_priv.h>

// >>> FIXME
#include <csr/core/path.h> // kio_env_xxx()
// <<< FIXME

////////////////////////////////////////////////////////////////////////////////////////////////////

#define applet_ptr() applet_mgr_get_applet()
#define applet_state_ptr() (&applet_ptr()->state)
#define applet_arena_ptr() (applet_state_ptr()->arena)

#define check_applet_initialized() check_expr(applet_ptr() && applet_ptr()->is_initialized)


////////////////////////////////////////////////////////////////////////////////
// logging
////////////////////////////////////////////////////////////////////////////////
void aio_log_message(enum log_level_type level, string_cstr fmt, ...)
{
    check_applet_initialized();

    check_expr(level <= LOG_LEVEL_MAX);
    check_ptr(fmt);

    struct arena *arena = applet_arena_ptr();
    check_ptr(arena);

    ////////////////////////////////////////

    va_list args;
    va_start(args, fmt);

    struct string message = string_create_vfmt(arena, fmt, args);

    va_end(args);

    ////////////////////////////////////////

    // FIXME scratch arena (pop strings after kio_log_message())
    string_cstr message_cstr = string_get_cstr(arena, message);
    string_cstr filename_cstr = string_get_cstr(arena, applet_get_filename(applet_ptr()));

    kio_log_message(level, filename_cstr, message_cstr);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////
// user config
////////////////////////////////////////////////////////////////////////////////
struct config* aio_get_config()
{
    check_applet_initialized();

    struct arena *arena = applet_arena_ptr();
    check_ptr(arena);

    ////////////////////////////////////////

    if (!applet_state_ptr()->config)
    {
        // string_cstr config_dir = kio_env_expand_var("APPLET_CONFIG_DIR");
        // string_cstr config_file = string_replace(arena, applet_get_filename(applet_ptr()), ".so", ".ini");
        // struct string config_path = string_create_fmt(arena, "%s/%s", config_dir, config_file);

        // >>> FIXME ugly stuff
        string_cstr path_cstr = NULL;
        {
            struct string filename = applet_get_filename(applet_ptr());
            filename = string_rchop(filename, string_rfind(filename, '.'));

            struct string config_dir = make_string_from_cstr(path_get("{APPLET_CONFIG_DIR}"));

            struct string path = string_create_fmt(arena, string_fmt"/"string_fmt".ini",
                string_fmt_arg(config_dir), string_fmt_arg(filename));

            path_cstr = string_get_cstr(arena, path);
        }
        // <<< FIXME

        klog_trace("loading ini file ... (%s)", path_cstr);

        applet_state_ptr()->config = config_create_from_ini(path_cstr);
    }

    return applet_state_ptr()->config;

error:
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// callbacks
////////////////////////////////////////////////////////////////////////////////
struct hid_callbacks* aio_get_hid_callbacks()
{
    check_applet_initialized();

    return &applet_state_ptr()->callbacks.hid;

error:
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// time
////////////////////////////////////////////////////////////////////////////////
struct clock* aio_get_clock()
{
    check_applet_initialized();

    return applet_state_ptr()->clock;

error:
    return NULL;
}

f64 aio_time_elapsed()
{
    check_applet_initialized();

    return clock_time_elapsed(applet_state_ptr()->clock);

error:
    return 0;
}

f64 aio_time_elapsed_delta()
{
    check_applet_initialized();

    return clock_time_elapsed_delta(applet_state_ptr()->clock);

error:
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// user interface
////////////////////////////////////////////////////////////////////////////////
result_e aio_add_ui_menu(string_cstr key, struct ui_menu *menu)
{
    check_applet_initialized();

    return ui_ctx_add_menu(applet_state_ptr()->ui, key, menu);

error:
    return RC_FAILURE;
}

struct ui_menu* aio_get_ui_menu(string_cstr key)
{
    check_applet_initialized();

    return ui_ctx_get_menu(applet_state_ptr()->ui, key);

error:
    return NULL;
}

result_e aio_add_ui_window(string_cstr  key, struct ui_window *window)
{
    check_applet_initialized();

    return ui_ctx_add_window(applet_state_ptr()->ui, key, window);

error:
    return RC_FAILURE;
}

struct ui_window* aio_get_ui_window(string_cstr  key)
{
    check_applet_initialized();

    return ui_ctx_get_window(applet_state_ptr()->ui, key);

error:
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// video
////////////////////////////////////////////////////////////////////////////////
static void _normalize_screen_create_info_values(struct screen_create_info *ci)
{
    check_ptr(ci);

    ////////////////////////////////////////

    // fallback to gpu surface type
    if (ci->surface.type == SCREEN_SURFACE_TYPE_UNKNOWN) {
        ci->surface.type = SCREEN_SURFACE_TYPE_GPU;
    }

    ////////////////////////////////////////

    // try to select to proper resize policy
    if (ci->resize_policy == SCREEN_RESIZE_POLICY_UNKNOWN)
    {
        // initially explicit resizing only
        ci->resize_policy = SCREEN_RESIZE_POLICY_EXPLICIT;

        // gpu surfaces can be automatically resized when needed (window resize, docking, ...)
        if (ci->surface.type == SCREEN_SURFACE_TYPE_GPU) {
            ci->resize_policy = SCREEN_RESIZE_POLICY_AUTO;
        }
    }

    ////////////////////////////////////////

    // try to select to proper scale policy
    if (ci->scale_policy == SCREEN_SCALE_POLICY_UNKNOWN)
    {
        // disable scaling by default
        ci->scale_policy = SCREEN_SCALE_POLICY_NONE;

        // use integer scaling for cpu surfaces
        if (ci->surface.type == SCREEN_SURFACE_TYPE_CPU) {
            ci->scale_policy = SCREEN_SCALE_POLICY_INTEGER;
        }
    }

    ////////////////////////////////////////

    // normalize screen surface
    struct xgl_viewport *viewport = &ci->surface.viewport;
    {
        // viewport size
        struct vec2 my_size = make_vec2(viewport->width, viewport->height);
        struct vec2 max_size = kio_video_get_window_resolution();

        if (my_size.w == 0 || my_size.h == 0)
        {
            // set default size
            my_size = make_vec2(SCREEN_WIDTH_MIN, SCREEN_HEIGHT_MIN);

            // gpu surfaces may be resized relatively to the os window size (ie. 75% resolution)
            if (ci->surface.type == SCREEN_SURFACE_TYPE_GPU) {
                my_size = vec2_scale(max_size, 0.75);
            }
        }

        viewport->width = clamp(my_size.w, SCREEN_WIDTH_MIN, max_size.w);
        viewport->height = clamp(my_size.h, SCREEN_HEIGHT_MIN, max_size.h);

        // depth range
        if (viewport->min_depth == viewport->max_depth)
        {
            // FIXME use globally defined values
            viewport->min_depth = 0.0f;
            viewport->max_depth = 1.0f;
        }
    }

    ////////////////////////////////////////

    // normalize scale factor
    if (ci->scale_factor < SCREEN_SCALE_MIN) {
        ci->scale_factor = SCREEN_SCALE_MIN;
    }

    // screen size matches ui window size, so no scaling needed at all
    if (ci->resize_policy == SCREEN_RESIZE_POLICY_AUTO) {
        ci->scale_factor = 1;
    }

error:
    return;
}

struct screen* aio_add_screen(string_cstr key, struct screen_create_info *ci)
{
    check_applet_initialized();

    check_ptr(key);

    check_expr(ui_ctx_get_window(applet_state_ptr()->ui, key) == NULL);

    ////////////////////////////////////////

    // create screen
    _normalize_screen_create_info_values(ci);

    struct screen *screen = screen_create(ci);
    check_ptr(screen);

    // show screen info
    string_cstr screen_name = screen_get_name(screen);
    struct vec2 screen_size = screen_get_size(screen);

    enum screen_surface_type surface_type = screen_get_surface_type(screen);
    string_cstr surface_type_str = screen_surface_type_cstr(surface_type);

    alog_info("adding screen (%s) ...", screen_name);
    alog_info(" - %s (%.0fx%.0f)", surface_type_str, screen_size.w, screen_size.h);

    ////////////////////////////////////////

    struct arena *arena = applet_arena_ptr();
    check_ptr(arena);

    // create window
    struct ui_window *window = calloc(1, sizeof(struct ui_window));
    check_mem(window);

    // >>> FIXME ugly stuff
    struct string applet_filename = applet_get_filename(applet_ptr());
    struct string win_title = string_create_fmt(arena, "%s##"string_fmt, screen_name, string_fmt_arg(applet_filename));

    ui_window_init(window, string_get_cstr(arena, win_title));
    ui_view_init(&window->view, UI_VIEW_TYPE_SCREEN, screen);
    // <<< FIXME

    // window and screen ownership go to the ui ctx (will be freed there)
    aio_add_ui_window(key, window);

    return screen;

error:
    return NULL;
}

struct screen* aio_get_screen(string_cstr key)
{
    check_applet_initialized();

    check_ptr(key);

    struct ui_window *window = ui_ctx_get_window(applet_state_ptr()->ui, key);
    check_ptr(window);

    struct screen *screen = window->view.user_data;
    check_ptr(screen);

    return screen;

error:
    return NULL;
}
