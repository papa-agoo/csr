////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define ctx_ptr() g_ctx

static struct ui_ctx *g_ctx = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _draw_applet_menu_cb(struct ui_menu *menu, struct ui_style *style)
{
    csr_assert(menu);
    csr_assert(style);

    igMenuItem_Bool("About", NULL, false, false);

    if (igMenuItem_Bool("Reload", NULL, false, true))
    {
        applet_mgr_request_applet_reload();
    }

    if (igMenuItem_Bool("Quit", NULL, false, true))
    {
        applet_mgr_request_applet_unload();
    }
}

static bool _cond_is_screen_window(struct ui_window *window, void *data)
{
    return window->view.type == UI_VIEW_TYPE_SCREEN;
}

static bool _cond_draw_screens_menu_cb(struct ui_menu *menu)
{
    // no window found means no screens available
    return ui_ctx_find_window(ctx_ptr(), _cond_is_screen_window, NULL) != NULL;
}

static void _on_draw_screen_window_menu_entry(struct ui_window *window, void *style)
{
    if (window->view.type == UI_VIEW_TYPE_SCREEN) {
        igMenuItem_BoolPtr(window->title, NULL, &window->is_opened, true);
    }
}

static void _draw_screens_menu_cb(struct ui_menu *menu, struct ui_style *style)
{
    csr_assert(menu);
    csr_assert(style);

    ui_ctx_traverse_windows(ctx_ptr(), _on_draw_screen_window_menu_entry, style);
}

////////////////////////////////////////////////////////////////////////////////

static void _register_menus()
{
    // applet
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "Applet");

        menu.draw_cb = _draw_applet_menu_cb;

        ui_ctx_add_menu(ctx_ptr(), "menu.applet", &menu);
    }

    // screens
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "Screens");

        menu.draw_cb = _draw_screens_menu_cb;
        menu.draw_cond_cb = _cond_draw_screens_menu_cb;

        ui_ctx_add_menu(ctx_ptr(), "menu.screens", &menu);
    }
}

void init_frontend_applet_ctx(struct ui_ctx *ctx)
{
    check_ptr(ctx);

    g_ctx = ctx;

    _register_menus();

error:
    return;
}
