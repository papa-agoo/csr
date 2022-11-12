////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define ctx_ptr() g_ctx

static struct ui_ctx *g_ctx = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _draw_application_menu_cb(struct ui_menu *menu, struct ui_style *style)
{
    csr_assert(menu);
    csr_assert(style);

    if (igMenuItem_Bool("Quit", NULL, false, true)) {
        klog_warn("event: quit application");
    }
}

static void _draw_kernel_menu_cb(struct ui_menu *menu, struct ui_style *style)
{
    csr_assert(menu);
    csr_assert(style);

    // console
    {
        struct ui_window *win = ui_ctx_get_window(ctx_ptr(), "window.console");
        csr_assert(win);

        igMenuItem_BoolPtr(win->title, NULL, &win->is_opened, false);
    }

    // log messages
    {
        struct ui_window *win = ui_ctx_get_window(ctx_ptr(), "window.log_messages");
        csr_assert(win);

        igMenuItem_BoolPtr(win->title, NULL, &win->is_opened, true);
    }
}

////////////////////////////////////////////////////////////////////////////////

static void _register_menus()
{
    // application
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "Application");

        menu.draw_cb = _draw_application_menu_cb;

        ui_ctx_add_menu(ctx_ptr(), "menu.application", &menu);
    }

    // kernel
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "Kernel");

        menu.draw_cb = _draw_kernel_menu_cb;

        ui_ctx_add_menu(ctx_ptr(), "menu.kernel", &menu);
    }
}

static void _register_windows()
{
    void init_console_view(struct ui_view * view);
    void init_log_messages_view(struct ui_view * view);

    // console
    {
        static struct ui_window window = {0};
        ui_window_init(&window, "Console", init_console_view);

        ui_ctx_add_window(ctx_ptr(), "window.console", &window);
    }

    // log messages
    {
        static struct ui_window window = {0};
        ui_window_init(&window, "Log Messages", init_log_messages_view);

        window.is_opened = true;

        ui_ctx_add_window(ctx_ptr(), "window.log_messages", &window);
    }
}

void init_frontend_application_ctx(struct ui_ctx *ctx)
{
    check_ptr(ctx);

    g_ctx = ctx;

    _register_menus();
    _register_windows();

error:
    return;
}
