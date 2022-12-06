////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/kernel/kio.h>
#include <csr/applet/aio.h>

#include <csr/applet/applet_mgr.h>
#include <csr/applet/applet_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define applet_ptr() applet_mgr_get_applet()
#define applet_state_ptr() (&applet_ptr()->state)

#define check_applet_initialized() check_expr(applet_ptr() && applet_ptr()->is_initialized)


////////////////////////////////////////////////////////////////////////////////
// logging
////////////////////////////////////////////////////////////////////////////////
void aio_log_message(enum log_level_type level, const char* message, ...)
{
    check_applet_initialized();

    check_expr(level <= LOG_LEVEL_MAX);
    check_ptr(message);

    ////////////////////////////////////////

    va_list args;
    va_start(args, message);

    va_list args_dupe;
    va_copy(args_dupe, args);

    size_t msg_len = vsnprintf(NULL, 0, message, args);
    char *msg_tmp = malloc(msg_len+1);

    vsnprintf(msg_tmp, msg_len + 1, message, args_dupe);

    va_end(args);
    va_end(args_dupe);

    ////////////////////////////////////////

    const char *applet_filename = applet_get_filename(applet_ptr());

    kio_log_message(level, applet_filename, msg_tmp);

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////
// user config
////////////////////////////////////////////////////////////////////////////////
struct config* aio_get_config()
{
    check_applet_initialized();

    klog_error("per applet configs not impl. yet :)");

    // return applet_state_ptr()->config;

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
result_e aio_add_ui_menu(const char *key, struct ui_menu *menu)
{
    check_applet_initialized();

    return ui_ctx_add_menu(applet_state_ptr()->ui, key, menu);

error:
    return RC_FAILURE;
}

struct ui_menu* aio_get_ui_menu(const char *key)
{
    check_applet_initialized();

    return ui_ctx_get_menu(applet_state_ptr()->ui, key);

error:
    return NULL;
}

result_e aio_add_ui_window(const char* key, struct ui_window *window)
{
    check_applet_initialized();

    return ui_ctx_add_window(applet_state_ptr()->ui, key, window);

error:
    return RC_FAILURE;
}

struct ui_window* aio_get_ui_window(const char* key)
{
    check_applet_initialized();

    return ui_ctx_get_window(applet_state_ptr()->ui, key);

error:
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// video
////////////////////////////////////////////////////////////////////////////////
struct screen* aio_add_screen(const char *key, struct screen_create_info *ci)
{
    check_applet_initialized();

    alog_error("aio_add_screen() not impl. yet");

    // 1) create screen
    // 2) create window
    // 3) ui_view_init(&window->view, UI_VIEW_TYPE_SCREEN, screen)

    // NOTES on fb resizing / scaling policies
    //  - screen CPU fb (pixelbuffer)
    //      - always "fixed size" framebuffer + use upscaling
    //  - screen GPU fb (XGL, OpenGL)
    //      - screen size params available
    //          - treat screen as "fixed size" framebuffer + use upscaling
    //      - no screen size params
    //          - use 75% of window size, framebuffer resizeable (no upscaling)

error:
    return NULL;
}

struct screen* aio_get_screen(const char *key)
{
    check_applet_initialized();

    alog_error("aio_get_screen() not impl. yet");

error:
    return NULL;
}
