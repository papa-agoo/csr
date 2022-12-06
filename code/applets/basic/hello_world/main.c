////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _on_kbd_key_pressed(struct keyboard_event *event)
{
    alog_trace("kbd key pressed : %s", keyboard_key_cstr(event->key));
}

static void _on_mouse_button_pressed(struct mouse_event *event)
{
    alog_trace("mouse button pressed : %s", mouse_button_cstr(event->button));
}

static void _on_mouse_wheel_spinned(struct mouse_event *event)
{
    alog_trace("mouse wheel spinned : x=%.2f, y=%.2f", event->wx, event->wy);
}

static void _on_mouse_moved(struct mouse_event *event)
{
    alog_trace("mouse moved : x=%d, y=%d (x_delta=%d, y_delta=%d)",
        event->x, event->y, event->x_delta, event->y_delta);
}

static void _draw_menu_command_cb(struct ui_menu *menu, struct ui_style *style)
{
    if (igMenuItem_Bool("Say Hello!", NULL, false, true)) {
        alog_info("Hello World!");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* _plugin_get_name()
{
    return "Hello World";
}

static const char* _plugin_get_description()
{
    return "Simple Example showing the Applet IO Layer in Action";
}

static struct version _plugin_get_version()
{
    return make_version(1, 0, 0);
}

static result_e _plugin_startup()
{
    // hid callbacks
    {
        struct hid_callbacks *hid = aio_get_hid_callbacks();
        hid->on_kbd_key_down = _on_kbd_key_pressed;
        hid->on_mouse_button_down = _on_mouse_button_pressed;
        hid->on_mouse_wheel_spin = _on_mouse_wheel_spinned;
        // hid->on_mouse_move = _on_mouse_moved;
    }

    // user interface
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "Command");

        menu.draw_cb = _draw_menu_command_cb;

        aio_add_ui_menu("menu.command", &menu);
    }

    ////////////////////////////////////////

    alog_warn("Enable Trace Messages to see HID Events :)");

    return RC_SUCCESS;
}

static void _plugin_shutdown()
{
    alog_info("I was running for %.2fs ...", aio_time_elapsed());
}

void applet_plugin_export(struct applet_plugin* plugin)
{
    plugin->get_name = _plugin_get_name;
    plugin->get_description = _plugin_get_description;
    plugin->get_version = _plugin_get_version;

    plugin->startup = _plugin_startup;
    plugin->shutdown = _plugin_shutdown;
}
