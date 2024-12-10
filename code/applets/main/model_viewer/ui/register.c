////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_draw_model_view();
void ui_draw_camera_view();
void ui_draw_rsx_view();

static void _draw_settings_view_cb(struct ui_view *view, struct ui_style *style)
{
    if (igBeginTabBar("applet.tabbar.settings", 0))
    {
        if (igBeginTabItem("Model", NULL, 0))
        {
            ui_draw_model_view();

            igEndTabItem();
        }

        if (igBeginTabItem("Camera", NULL, 0))
        {
            ui_draw_camera_view();

            igEndTabItem();
        }

        if (igBeginTabItem("RSX", NULL, 0))
        {
            ui_draw_rsx_view();

            igEndTabItem();
        }

        igEndTabBar();
    }
}

static void _draw_view_menu_cb(struct ui_menu *menu, struct ui_style *style)
{
    csr_assert(menu);
    csr_assert(style);

    // settings window
    {
        struct ui_window *win = aio_get_ui_window("applet.win.settings");
        csr_assert(win);

        igMenuItem_BoolPtr(win->title, NULL, &win->is_opened, true);
    }
}

result_e model_viewer_register_ui()
{
    // view menu
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "View");

        menu.draw_cb = _draw_view_menu_cb;

        check_expr(aio_add_ui_menu("applet.menu.view", &menu) == RC_SUCCESS);
    }

    // settings window
    {
        static struct ui_window window = {0};
        ui_window_init(&window, "Settings");

        window.view.draw_cb = _draw_settings_view_cb;
        window.is_opened = true;

        check_expr(aio_add_ui_window("applet.win.settings", &window) == RC_SUCCESS);
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
