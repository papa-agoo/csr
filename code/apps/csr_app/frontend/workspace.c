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

    bool has_app_focus = application_has_input_focus();

    struct vec4 color = (has_app_focus) ? style->color.menu_head_on : style->color.menu_head_off;

    igTextColored(make_ImVec4_from_vec4(color), ENV_APP_NAME);

error:
    return;
}

static void _draw_menu_tail(struct ui_style *style)
{
    check_ptr(style);

    struct vec2 fb_res = kio_video_get_window_resolution();

    f32 cursor_pos = fb_res.w;
    f32 margin_right = 8.0;

    ////////////////////////////////////////

    // FIXME arena_scratch_begin()
    struct arena_scratch scratch = {0};
    scratch.arena = kio_mem_get_frame_arena();
    scratch.position = arena_get_current_position(scratch.arena);

    // applet state
    {
        struct vec4 color = style->color.applet_unloaded;
        string_cstr text = "no applet loaded";

        struct applet *applet = applet_mgr_get_applet();

        if (applet)
        {
            color = style->color.applet_loaded;
            text = string_get_cstr(scratch.arena, applet_get_filename(applet));
        }

        struct ImVec2 text_size = {0};
        igCalcTextSize(&text_size, text, NULL, false, -1.0f);

        cursor_pos -= text_size.x + margin_right;
        igSetCursorPosX(cursor_pos);

        igTextColored(make_ImVec4_from_vec4(color), text);
    }

    // FIXME arena_scratch_end()
    arena_pop_to(scratch.arena, scratch.position);

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
    if (application_ptr()->show_main_menu && igBeginMainMenuBar())
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
