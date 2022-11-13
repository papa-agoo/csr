////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void on_draw_windows(struct ui_ctx *ctx, void *style)
{
    ui_ctx_traverse_windows(ctx, (ui_ctx_traverse_windows_cb_t)ui_window_draw, style);
}

static void on_draw_menus(struct ui_ctx *ctx, void *style)
{
    igSeparator();

    ui_ctx_traverse_menus(ctx, (ui_ctx_traverse_menus_cb_t)ui_menu_draw, style);

error:
    return;
}

static void _draw_menu_head(struct ui_style *style)
{
    check_ptr(style);

    bool has_app_focus = false; // FIXME

    struct vec4 color = (has_app_focus) ? style->color.menu_head_on : style->color.menu_head_off;

    igTextColored(make_ImVec4_from_vec4(color), ENV_APP_NAME);

error:
    return;
}

static void _draw_menu_tail(struct ui_style *style)
{
    check_ptr(style);

    // application state
    // ...

    ////////////////////////////////////////

    // applet state
    bool is_applet_loaded = false; // FIXME

    struct vec4 color = (is_applet_loaded) ? style->color.applet_loaded : style->color.applet_unloaded;

    // igTextColored(make_ImVec4_from_vec4(color), "foo.so");

error:
    return;
}

void on_workspace_main_tick(struct ui_workspace *workspace)
{
    struct ui_style *style = (void*) ui_get_theme_style();
    check_ptr(style);

    ////////////////////////////////////////

    // draw windows
    ui_workspace_traverse_contexts(workspace, on_draw_windows, style);

    ////////////////////////////////////////

    // [ head ] [ application menus | applet menus ] <- ... -> [tail]

    // draw main menu
    if (igBeginMainMenuBar())
    {
        // menu head
        _draw_menu_head(style);

        // menu 
        ui_workspace_traverse_contexts(workspace, on_draw_menus, style);

        // menu tail
        _draw_menu_tail(style);

        igEndMainMenuBar();
    }
error:
    return;
}
