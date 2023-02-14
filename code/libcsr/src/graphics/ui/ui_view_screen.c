////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/screen.h>
#include <csr/graphics/ui/ui_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _center_image(struct ImVec2 image_size)
{
    // https://github.com/ocornut/imgui/issues/2212

    struct ImVec2 content_region = {0};
    igGetContentRegionAvail(&content_region);

    struct ImVec2 cursor_pos = {0};
    igGetCursorPos(&cursor_pos);

    // center cursor within content region. rounding prevents line artifacts.
    cursor_pos.x += ceilf((content_region.x - image_size.x) * 0.5f);
    cursor_pos.y += ceilf((content_region.y - image_size.y) * 0.5f);

    igSetCursorPos(cursor_pos);
}

static void _draw_image(xgl_texture texture, u32 width, u32 height)
{
    ImTextureID tex_id = xgl_get_texture_handle(texture);
    struct ImVec2 image_size = {width, height};

    struct ImVec2 uv0 = {0, 1};
    struct ImVec2 uv1 = {1, 0};
    struct ImVec4 tint_color = {1, 1, 1, 1};
    struct ImVec4 border_color = {0.5, 0.5, 0.5, 0.5};

    igImage(tex_id, image_size, uv0, uv1, tint_color, border_color);
}

static void _draw_view(struct ui_view* view, struct ui_style *style)
{
    struct screen* screen = view->user_data;

    // sync window / screen area
    {
        struct ImVec2 c = {0};
        igGetContentRegionAvail(&c);

        // content region (minus 1px border on each side)
        struct vec2 content_region = make_vec2(c.x - 2, c.y - 2);

        // screen size
        struct vec2 scaled_size = screen_get_scaled_size(screen);

        // update size
        if (!vec2_equal(content_region, scaled_size))
        {
            // FIXME incorporate resize / scale policies
            screen_set_size(screen, content_region);
        }
    }

    ////////////////////////////////////////

    // _update_screen_size();

    struct vec2 size = screen_get_size(screen);
    struct vec2 scaled_size = screen_get_scaled_size(screen);

    // _center_image(scaled_size, size);

    xgl_texture texture = screen_get_texture(screen);

    _draw_image(texture, scaled_size.w, scaled_size.h);
}

static void _push_parent_properties_cb(struct ui_window* win) {}
static void _pop_parent_properties_cb(struct ui_window* win) {}

void init_screen_view(struct ui_view* view, void *screen)
{
    check_ptr(view);
    check_ptr(screen);

    // setup view
    view->type = UI_VIEW_TYPE_SCREEN;
    view->draw_cb = _draw_view;
    view->user_data = screen;

    // setup parent
    struct ui_window *window = view->parent;
    check_ptr(window);

    window->is_opened = true;

    window->priv.push_properties_cb = _push_parent_properties_cb;
    window->priv.pop_properties_cb = _pop_parent_properties_cb;

error:
    return;
}
