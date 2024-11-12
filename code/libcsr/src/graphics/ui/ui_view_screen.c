////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/screen.h>
#include <csr/graphics/ui/ui_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void draw_screen_context_menu(struct ui_window *window)
{
    struct screen* screen = window->view.user_data;

    igPushStyleVar_Vec2(ImGuiStyleVar_ItemSpacing, make_ImVec2(8, 8));
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, make_ImVec2(8, 8));

    ////////////////////////////////////////

    ImGuiPopupFlags flags = ImGuiPopupFlags_MouseButtonRight;

    if (igBeginPopupContextWindow("screen.ctx.menu", flags))
    {
        enum screen_scale_policy scale_policy = screen_get_scale_policy(screen);
        enum screen_resize_policy resize_policy = screen_get_resize_policy(screen);

        if (igMenuItem_Bool("Fullscreen", NULL, false, false)) {
            // FIXME
        }

        if (igMenuItem_Bool("Fit Window to Screen", NULL, false, true))
        {
            igSetWindowSize_Vec2(make_ImVec2_zero(), ImGuiCond_Always);

            window->hint.fit_window_to_content_size = true;
        }

        igSeparator();

        ////////////////////////////////////////

        {
            bool enable_scaling_options = scale_policy == SCREEN_SCALE_POLICY_INTEGER;

            bool has_reached_max_scale = screen_is_scale_maxed(screen);

            if (igMenuItem_Bool("Scale Up", NULL, false, !has_reached_max_scale && enable_scaling_options)) {
                screen_scale_up(screen);
            }

            bool has_reached_min_scale = screen_get_scale(screen) == 1;

            if (igMenuItem_Bool("Scale Down", NULL, false, !has_reached_min_scale && enable_scaling_options)) {
                screen_scale_down(screen);
            }

            enable_scaling_options = scale_policy != SCREEN_SCALE_POLICY_NONE;

            if (igMenuItem_Bool("Scale Reset", NULL, false, !has_reached_min_scale && enable_scaling_options)) {
                screen_reset_scale(screen);
            }
        }

        igSeparator();

        ////////////////////////////////////////

        if (igBeginMenu("Resize Policy", true))
        {
            enum screen_resize_policy my_resize_policy = SCREEN_RESIZE_POLICY_AUTO;

            if (igMenuItem_Bool("Automatic", NULL, resize_policy == my_resize_policy, true)) {
                screen_set_resize_policy(screen, my_resize_policy);
            }

            my_resize_policy = SCREEN_RESIZE_POLICY_EXPLICIT;

            if (igMenuItem_Bool("Explicit", NULL, resize_policy == my_resize_policy, true)) {
                screen_set_resize_policy(screen, my_resize_policy);
            }

            igEndMenu();
        }

        ////////////////////////////////////////

        if (igBeginMenu("Scale Policy", resize_policy == SCREEN_RESIZE_POLICY_EXPLICIT))
        {
            enum screen_scale_policy my_scale_policy = SCREEN_SCALE_POLICY_NONE;

            if (igMenuItem_Bool("None", NULL, scale_policy == my_scale_policy, true)) {
                screen_set_scale_policy(screen, my_scale_policy);
            }

            my_scale_policy = SCREEN_SCALE_POLICY_FP;

            if (igMenuItem_Bool("Floating Point", NULL, scale_policy == my_scale_policy, true)) {
                screen_set_scale_policy(screen, my_scale_policy);
            }

            my_scale_policy = SCREEN_SCALE_POLICY_INTEGER;

            if (igMenuItem_Bool("Integer", NULL, scale_policy == my_scale_policy, true)) {
                screen_set_scale_policy(screen, my_scale_policy);
            }

            igEndMenu();
        }

        ////////////////////////////////////////

        bool keep_aspect_ratio = screen_get_keep_aspect_ratio(screen);
        bool enable_keep_ar_menu = resize_policy == SCREEN_RESIZE_POLICY_AUTO;

        if (igMenuItem_Bool("Keep Aspect Ratio", NULL, keep_aspect_ratio, enable_keep_ar_menu)) {
            screen_toggle_keep_aspect_ratio(screen);
        }

        ////////////////////////////////////////

        igSeparator();

        igMenuItem_BoolPtr("Show Surface Info", NULL, &window->hint.show_surface_info, true);

        igEndPopup();
    }

    ////////////////////////////////////////

    igPopStyleVar(2);
}

// FIXME relpace with custom xgl font renderer
static void draw_surface_info(struct screen *screen)
{
    string_cstr surface_type_str = screen_get_surface_type_cstr(screen);

    struct vec2 size = screen_get_size(screen);

    f32 scale_factor = screen_get_scale(screen);
    struct vec2 size_scaled = make_vec2(size.w * scale_factor, size.h * scale_factor);

    const struct screen_stats *stats = screen_get_stats(screen);

    // FIXME make_string("", ...)
    char str_buf[128];

    snprintf(str_buf, 128, "%s Surface : %.0fx%.0f @ %.2fx (%.0fx%.0f, aspect: %.2f), %05.2fms (%02.0ffps)",
        surface_type_str, size.w, size.h, scale_factor, size_scaled.w, size_scaled.h, screen_get_aspect_ratio(screen),
        stats->avg_frametime_ms, stats->avg_fps);

    // poor mans text shadows :)
    {
        f32 padding = 25;
        f32 shadow_offset = 1;

        struct ImVec2 cursor_pos = {0};
        igGetCursorPos(&cursor_pos);

        // bg text
        cursor_pos.x = padding;
        cursor_pos.y = padding + igGetFrameHeight();

        igSetCursorPos(cursor_pos);
        igTextColored(make_ImVec4(0, 0, 0, 1), "%s", str_buf);

        // fg text
        cursor_pos.x -= shadow_offset;
        cursor_pos.y -= shadow_offset;

        igSetCursorPos(cursor_pos);
        igText("%s", str_buf);
    }

error:
    return;
}

static void update_screen_size(struct screen *screen, struct vec2 content_size)
{
    struct vec2 size_old = screen_get_size(screen);
    struct vec2 size_new = screen_get_size_for_parent(screen, content_size);

    if (!vec2_equal(size_old, size_new)) {
        screen_set_size(screen, size_new);
    }
}

static void update_screen_scale(struct screen *screen, struct vec2 content_size)
{
    f32 scale_old = screen_get_scale(screen);
    f32 scale_new = screen_get_max_scale_for_parent(screen, content_size);

    if (scale_old != scale_new) {
        screen_set_scale(screen, scale_new);
    }
}

static void draw_screen_image(xgl_texture texture, struct vec2 screen_size, struct vec2 content_size)
{
    // center image
    {
        // https://github.com/ocornut/imgui/issues/2212

        struct ImVec2 cursor_pos = {0};
        igGetCursorPos(&cursor_pos);

        // center cursor within content region. rounding prevents line artifacts.
        cursor_pos.x += ceilf((content_size.x - screen_size.x) * 0.5f);
        cursor_pos.y += ceilf((content_size.y - screen_size.y) * 0.5f);

        igSetCursorPos(cursor_pos);
    }

    // draw image
    {
        ImTextureID tex_id = xgl_get_texture_handle(texture);

        struct ImVec2 uv0 = {0, 1};
        struct ImVec2 uv1 = {1, 0};
        struct ImVec4 tint_color = {1, 1, 1, 1};
        struct ImVec4 border_color = {0.5, 0.5, 0.5, 0.0};

        igImage(tex_id, make_ImVec2_from_vec2(screen_size), uv0, uv1, tint_color, border_color);
    }
}

static void _draw_view(struct ui_view* view, struct ui_style *style)
{
    struct ui_window *window = view->parent;
    struct screen* screen = view->user_data;

    // suspend the screen if the window is closed
    screen_suspend(screen, !window->is_opened);

    ////////////////////////////////////////

    if (window->hint.fit_window_to_content_size) {
        igSetWindowSize_Vec2(make_ImVec2_zero(), ImGuiCond_Always);
    }

    struct vec2 screen_size = {0};

    struct ImVec2 content_size = {0};
    igGetContentRegionAvail(&content_size);

    struct vec2 content_size_old = window->priv.content_size;
    struct vec2 content_size_new = make_vec2(content_size.x, content_size.y);

    if (vec2_equal(content_size_old, make_vec2_zero()))
    {
        // frame 1: do not apply resizing / scaling yet, to ...
        //  - allow imgui to load and apply the config of this window (if present)
        //  - otherwise to calc initial values and save them to the config
        screen_size = screen_get_scaled_size(screen);
        content_size_new = screen_size;
    }
    else
    {
        // frame 1 + N : the window has it's initial config, so ...
        //  - it's safe now to do our resizing / scaling stuff
        if (screen_get_resize_policy(screen) == SCREEN_RESIZE_POLICY_AUTO)
        {
            // update screen size
            update_screen_size(screen, content_size_new);
            screen_size = screen_get_size(screen);
        }
        else
        {
            // update scale factor (scaled size)
            update_screen_scale(screen, content_size_new);
            screen_size = screen_get_scaled_size(screen);
        }

        // fit window to screen size
        if (window->hint.fit_window_to_content_size)
        {
            window->hint.fit_window_to_content_size = false;

            content_size_new = screen_size;
        }
    }

    ////////////////////////////////////////

    xgl_texture texture = screen_get_texture(screen);
    draw_screen_image(texture, screen_size, content_size_new);

    window->priv.content_size = content_size_new;

    ////////////////////////////////////////

    // FIXME need external check (ie. application_has_input_focus())
    if (igIsKeyDown_Nil(ImGuiKey_LeftShift)) {
        draw_screen_context_menu(window);
    }

    if (window->hint.show_surface_info) {
        draw_surface_info(screen);
    }
}

static void _on_window_resize()
{
    clog_info("_on_window_resize()");
}

static void _on_screen_resize()
{
    clog_info("_on_screen_resize()");
}

static void _draw_view_new(struct ui_view* view, struct ui_style *style)
{
    // content_size_old != content_size_new
    bool window_size_changed = false;

    if (window_size_changed) {
        _on_window_resize();
    }
    else
    {
        // !window_size_changed && content_size != screen_size
        bool screen_size_changed = false;

        if (screen_size_changed) {
            _on_screen_resize();
        }
    }
}

static void _push_parent_properties_cb(struct ui_window* win)
{
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, make_ImVec2(0, 0));
    igPushStyleVar_Vec2(ImGuiStyleVar_FramePadding, make_ImVec2(8, 8));
    igPushStyleVar_Float(ImGuiStyleVar_ChildBorderSize, 0);
}

static void _pop_parent_properties_cb(struct ui_window* win)
{
    igPopStyleVar(3);
}

void init_screen_view(struct ui_view* view, void *screen)
{
    check_ptr(view);
    check_ptr(screen);

    // setup view
    view->type = UI_VIEW_TYPE_SCREEN;
    view->draw_cb = _draw_view;
    // view->draw_cb = _draw_view_new;
    view->user_data = screen;

    // setup parent
    struct ui_window *window = view->parent;
    check_ptr(window);

    window->is_opened = true;

    window->priv.flags |= ImGuiWindowFlags_NoCollapse;
    window->priv.flags |= ImGuiWindowFlags_NoScrollbar;

    window->priv.push_properties_cb = _push_parent_properties_cb;
    window->priv.pop_properties_cb = _pop_parent_properties_cb;

error:
    return;
}
