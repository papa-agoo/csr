////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet/applet_priv.h>
#include <csr/applet/applet_state.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct applet* applet_create(struct string path)
{
    check_expr(string_is_valid(path));

    struct applet* applet = calloc(1, sizeof(struct applet));
    check_mem(applet);

    // load plugin
    check_result(applet_plugin_load(&applet->plugin, path));

    applet_state_init(applet);

    return applet;

error:

    if (applet) {
        applet_destroy(applet);
    }

    return NULL;
}

void applet_destroy(struct applet* applet)
{
    check_ptr(applet);

    if (applet->is_running) {
        clog_warn("applet is still running!?");
    }

    applet_state_quit(applet);

    applet_plugin_unload(&applet->plugin);

    free(applet);

error:
    return;
}

result_e applet_startup(struct applet* applet)
{
    check_ptr(applet);

    check_expr(!applet->is_running);
    check_expr(applet->is_initialized);

    ////////////////////////////////////////

    clock_init(applet->state.clock, 0);

    check_result(applet->plugin.startup());

    ////////////////////////////////////////

    applet->is_running = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void applet_shutdown(struct applet* applet)
{
    check_ptr(applet);

    if (applet->is_running)
    {
        applet->plugin.shutdown();

        applet->is_running = false;
    }

error:
    return;
}

void applet_tick(struct applet* applet, f64 dt)
{
    check_ptr(applet);
    check_expr(applet->is_running);

    clock_tick(applet->state.clock, dt);

    applet->plugin.tick();

    arena_reset(applet->state.allocator.arena_frame);

error:
    return;
}

struct string applet_get_name(struct applet* applet)
{
    check_ptr(applet);

    return applet->plugin.get_name();

error:
    return make_string("<unknown>");
}

struct string applet_get_description(struct applet* applet)
{
    check_ptr(applet);

    return applet->plugin.get_description();

error:
    return make_string("<unknown>");
}

struct string applet_get_version_str(struct applet* applet)
{
    check_ptr(applet);

    return version_str(applet->plugin.get_version());

error:
    return make_string("<unknown>");
}

struct string applet_get_filename(struct applet* applet)
{
    check_ptr(applet);

    return applet->plugin.filename;

error:
    return make_string("<unknown>");
}

struct ui_ctx* applet_get_ui_ctx(struct applet *applet)
{
    check_ptr(applet);

    csr_assert(applet->is_initialized);

    return applet->state.ui;

error:
    return NULL;
}

struct hid_callbacks* applet_get_hid_callbacks(struct applet *applet)
{
    check_ptr(applet);

    csr_assert(applet->is_initialized);

    return &applet->state.callbacks.hid;

error:
    return NULL;
}
