////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/file_io.h>
#include <csr/core/memory/arena.h>

#include <csr/applet/applet_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void applet_state_init(struct applet *applet)
{
    check_ptr(applet);

    csr_assert(!applet->is_initialized);

    ////////////////////////////////////////

    struct applet_state *state = &applet->state;

    // arena allocator
    state->arena = make_arena();
    check_ptr(state->arena);

    // clock
    state->clock = clock_create("VTC");
    check_ptr(state->clock);

    // ui
    state->ui = ui_ctx_create("Applet");
    check_ptr(state->ui);

    // callbacks
    state->callbacks.hid = (struct hid_callbacks){0};

    ////////////////////////////////////////

    applet->is_initialized = true;

error:
    return;
}

static void _destroy_screen_windows_cb(struct ui_window *window, void *data)
{
    check_ptr(window);

    if (window->view.type == UI_VIEW_TYPE_SCREEN) {
        screen_destroy(window->view.user_data);
    }

error:
    return;
}

void applet_state_quit(struct applet *applet)
{
    check_ptr(applet);

    csr_assert(applet->is_initialized);

    ////////////////////////////////////////

    struct applet_state *state = &applet->state;

    // callbacks
    state->callbacks.hid = (struct hid_callbacks){0};

    // ui
    ui_ctx_traverse_windows(state->ui, _destroy_screen_windows_cb, NULL);
    ui_ctx_destroy(state->ui);

    // config
    if (state->config)
    {
        // create the config dir
        struct string config_path = config_get_filename(state->config);
        struct string config_dir = fio_fs_get_parent_path(config_path);

        if (!fio_fs_exists(config_dir)) {
            fio_fs_create_directory(config_dir);
        }

        // save config file
        config_flush(state->config);
        config_destroy(state->config);
    }

    // clock
    clock_destroy(state->clock);

    // arena
    arena_destroy(state->arena);

    ////////////////////////////////////////

    applet->is_initialized = false;

error:
    return;
}
