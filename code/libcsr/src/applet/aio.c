////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/kernel/kio.h>
#include <csr/applet/aio.h>

#include <csr/applet/applet_mgr.h>
#include <csr/applet/applet_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define applet_ptr() applet_mgr_get_applet()

#define applet_state_ptr() (&applet_ptr()->state)
#define applet_plugin_ptr() (&applet_ptr()->plugin)

#define applet_arena_main_ptr() (applet_state_ptr()->allocator.arena_main)
#define applet_arena_frame_ptr() (applet_state_ptr()->allocator.arena_frame)

#define check_applet_initialized() check_expr(applet_ptr() && applet_ptr()->is_initialized)


////////////////////////////////////////////////////////////////////////////////
// logging
////////////////////////////////////////////////////////////////////////////////
void aio_log_message(enum log_level_type level, string_cstr fmt, ...)
{
    check_applet_initialized();

    check_expr(level <= LOG_LEVEL_MAX);
    check_ptr(fmt);

    struct arena *arena_frame = applet_arena_frame_ptr();
    check_ptr(arena_frame);

    ////////////////////////////////////////

    va_list args;
    va_start(args, fmt);

    struct string message = string_create_vfmt(arena_frame, fmt, args);

    va_end(args);

    ////////////////////////////////////////

    string_cstr message_cstr = string_get_cstr(arena_frame, message);
    string_cstr filename_cstr = string_get_cstr(arena_frame, applet_plugin_ptr()->filename);

    kio_log_message(level, filename_cstr, message_cstr);

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////
// env vars
////////////////////////////////////////////////////////////////////////////////
struct string aio_env_get_var(string_cstr name)
{
    return kio_env_get_var(name);
}

string_cstr aio_env_get_var_cstr(string_cstr name)
{
    return kio_env_get_var_cstr(name);
}

struct string aio_env_expand_str(string_cstr str)
{
    return kio_env_expand_str(str);
}

string_cstr aio_env_expand_str_cstr(string_cstr str)
{
    return kio_env_expand_str_cstr(str);
}


////////////////////////////////////////////////////////////////////////////////
// memory
////////////////////////////////////////////////////////////////////////////////
struct arena* aio_get_main_arena()
{
    return applet_arena_main_ptr();
}

struct arena* aio_get_frame_arena()
{
    return applet_arena_frame_ptr();
}


////////////////////////////////////////////////////////////////////////////////
// user config
////////////////////////////////////////////////////////////////////////////////
struct config* aio_get_config()
{
    check_applet_initialized();

    struct arena *arena = applet_arena_main_ptr();
    check_ptr(arena);

    ////////////////////////////////////////

    if (!applet_state_ptr()->config)
    {
        // construct config path
        struct string config_dir = kio_env_expand_str("{APPLET_CONFIG_DIR}");
        struct string config_file = string_replace(arena, applet_plugin_ptr()->filename, make_string(".so"), make_string(".ini"));

        struct string config_path = string_create_fmt(arena, "%S/%S", &config_dir, &config_file);

        // load config
        klog_trace("loading ini file ... (%S)", &config_path);

        applet_state_ptr()->config = config_create_from_ini(config_path);
    }

    return applet_state_ptr()->config;

error:
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// hid
////////////////////////////////////////////////////////////////////////////////
struct hid_callbacks* aio_get_hid_callbacks()
{
    check_applet_initialized();

    return &applet_state_ptr()->callbacks.hid;

error:
    return NULL;
}

bool aio_hid_kbd_key_down(enum keyboard_key key)
{
    return kio_hid_kbd_key_down(key);
}

bool aio_hid_mouse_button_down(enum mouse_button button)
{
    return kio_hid_mouse_button_down(button);
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
#define _get_unique_ui_name(name) \
    ((string_cstr)string_create_fmt(applet_arena_main_ptr(), "%s##%S", name, &applet_plugin_ptr()->filename).ptr)

result_e aio_add_ui_menu(string_cstr key, struct ui_menu *menu)
{
    check_applet_initialized();

    check_ptr(menu);
    check_ptr(menu->title);

    // make menu name unique (ie. My Entry -> My Entry##my_applet.so)
    menu->title = _get_unique_ui_name(menu->title);

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

    check_ptr(window);
    check_ptr(window->title);

    // make window name unique (ie. My Window -> My Window##my_applet.so)
    window->title = _get_unique_ui_name(window->title);

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

    // try to select the proper resize policy
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

    struct arena *arena = applet_arena_main_ptr();
    check_ptr(arena);

    // create screen
    _normalize_screen_create_info_values(ci);

    struct screen *screen = screen_create(ci);
    check_ptr(screen);

    // show screen info
    struct string screen_name = screen_get_name(screen);
    struct vec2 screen_size = screen_get_size(screen);

    enum screen_surface_type surface_type = screen_get_surface_type(screen);
    string_cstr surface_type_cstr = screen_surface_type_cstr(surface_type);

    alog_info("adding screen (%S) ...", &screen_name);
    alog_info(" - %s (%.0fx%.0f)", surface_type_cstr, screen_size.w, screen_size.h);

    ////////////////////////////////////////

    // create window
    struct ui_window *window = calloc(1, sizeof(struct ui_window));
    check_mem(window);

    ui_window_init(window, string_get_cstr(arena, screen_name));
    ui_view_init(&window->view, UI_VIEW_TYPE_SCREEN, screen);

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
