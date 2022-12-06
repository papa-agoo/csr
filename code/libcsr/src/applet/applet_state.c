////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet/applet_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void applet_state_init(struct applet *applet)
{
    check_ptr(applet);

    csr_assert(!applet->is_initialized);

    ////////////////////////////////////////

    struct applet_state *state = &applet->state;

    // clock
    state->clock = clock_create("VTC");
    check_ptr(state->clock);

    // config
    state->config = config_create();
    check_ptr(state->config);

    // ui
    state->ui = ui_ctx_create("Applet");
    check_ptr(state->ui);

    // screens
    state->screens = hashmap_create();
    check_ptr(state->screens);

    // callbacks
    state->callbacks.hid = (struct hid_callbacks){0};

    ////////////////////////////////////////

    applet->is_initialized = true;

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

    // screens
    hashmap_destroy(state->screens);

    // ui
    ui_ctx_destroy(state->ui);

    // config
    config_destroy(state->config);

    // clock
    clock_destroy(state->clock);

    ////////////////////////////////////////

    applet->is_initialized = false;

error:
    return;
}
