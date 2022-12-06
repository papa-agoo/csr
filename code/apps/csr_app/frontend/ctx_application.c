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

    // applet db
    {
        struct ui_window *win = ui_ctx_get_window(ctx_ptr(), "window.applet_db");
        csr_assert(win);

        igMenuItem_BoolPtr(win->title, NULL, &win->is_opened, true);
    }

    // settings
    {
        struct ui_window *win = ui_ctx_get_window(ctx_ptr(), "window.settings");
        csr_assert(win);

        igMenuItem_BoolPtr(win->title, NULL, &win->is_opened, true);
    }

    igSeparator();

    // quit
    if (igMenuItem_Bool("Quit", NULL, false, true)) {
        application_stop();
    }
}

static void _draw_kernel_menu_cb(struct ui_menu *menu, struct ui_style *style)
{
    csr_assert(menu);
    csr_assert(style);

    // log messages
    {
        struct ui_window *win = ui_ctx_get_window(ctx_ptr(), "window.log_messages");
        csr_assert(win);

        igMenuItem_BoolPtr(win->title, NULL, &win->is_opened, true);
    }
}

static void _draw_help_menu_cb(struct ui_menu *menu, struct ui_style *style)
{
    csr_assert(menu);
    csr_assert(style);

    igMenuItem_BoolPtr("About", NULL, NULL, false);

    igSeparator();

    if (igMenuItem_BoolPtr("ImGui Demo", NULL, NULL, true)) {
        ui_toggle_imgui_demo_window();
    }
}

static void _on_draw_windows_menu_entry(struct ui_window *window, void *data)
{
    csr_assert(window);

    if (!window->is_opened) return;

    igNewLine();
    igSameLine(0, 15);

    if (igMenuItem_Bool(window->title, NULL, ui_window_is_focused(window), true))
    {
        ui_window_set_focused(window);
    }
}

static void _on_draw_windows_menu_group(struct ui_ctx *ctx, void *data)
{
    check_ptr(ctx);
    check_ptr(data);

    struct ui_style *style = data;

    if (ui_ctx_get_window_count(ctx) > 0)
    {
        struct vec4 color = style->color.menu_group_title;

        igTextColored(make_ImVec4_from_vec4(color), "%s", ui_ctx_get_name(ctx));

        ui_ctx_traverse_windows(ctx, _on_draw_windows_menu_entry, style);
    }

error:
    return;
}

static void _draw_windows_menu_cb(struct ui_menu *menu, struct ui_style *style)
{
    // windows menu
    //
    //  group 1 (Application)
    //      entry 1.1 (Window)
    //      entry 1.2 (Window)
    //
    //  group 2 (Applet)
    //      entry 2.1 (Window)

    struct ui_workspace *workspace = ui_get_workspace();
    csr_assert(workspace);

    ui_workspace_traverse_contexts(workspace, _on_draw_windows_menu_group, style);
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

    // windows
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "Windows");

        menu.draw_cb = _draw_windows_menu_cb;

        ui_ctx_add_menu(ctx_ptr(), "menu.windows", &menu);
    }

    // help
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "Help");

        menu.draw_cb = _draw_help_menu_cb;

        ui_ctx_add_menu(ctx_ptr(), "menu.help", &menu);
    }
}

static void _register_windows()
{
    void init_applet_db_view(struct ui_view * view);
    void init_settings_view(struct ui_view * view);
    void init_log_messages_view(struct ui_view * view);

    // applet db
    {
        static struct ui_window window = {0};
        ui_window_init(&window, "Applet Database", init_applet_db_view);

        window.flags = ImGuiWindowFlags_NoDocking;

        ui_ctx_add_window(ctx_ptr(), "window.applet_db", &window);
    }

    // settings
    {
        static struct ui_window window = {0};
        ui_window_init(&window, "Settings", init_settings_view);

        window.flags = ImGuiWindowFlags_NoDocking;

        ui_ctx_add_window(ctx_ptr(), "window.settings", &window);
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
