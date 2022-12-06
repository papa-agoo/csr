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

    if (igMenuItem_Bool("Quit", NULL, false, true))
    {
        applet_mgr_request_applet_unload();
    }
}

////////////////////////////////////////////////////////////////////////////////

static void _register_menus()
{
    // applet
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "Applet");

        menu.draw_cb = _draw_applet_menu_cb;

        ui_ctx_add_menu(ctx_ptr(), "menu.main", &menu);
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
