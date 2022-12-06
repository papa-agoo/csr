////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui/cimgui.h>
#include <csr/graphics/ui/theme_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _on_apply_theme(struct ui_theme *theme)
{
    igStyleColorsDark(NULL);
}

struct ui_theme* ui_theme_imgui_dark_ptr()
{
    static struct ui_theme theme = {
        .info = {
            .name = "ImGui Dark",
            .author = "Omar Cornut",
            .version = make_version(1, 0, 0),

            // FIXME use appropriate colors :)
            .style = {
                .color = {
                    .background         = make_vec4(0.45f, 0.55f, 0.60f, 1.00f),

                    .log_info           = make_vec4(0.75f, 0.75f, 0.75f, 1.00f),
                    .log_notice         = make_vec4(0.44f, 0.85f, 0.85f, 1.00f),
                    .log_warn           = make_vec4(1.00f, 0.60f, 0.40f, 1.00f),
                    .log_error          = make_vec4(1.00f, 0.43f, 0.35f, 1.00f),
                    .log_trace          = make_vec4(0.74f, 0.50f, 0.73f, 1.00f),

                    .menu_head_on       = make_vec4(0.14f, 0.75f, 0.55f, 1.00f),
                    .menu_head_off      = make_vec4(0.55f, 0.55f, 0.55f, 1.00f),
                    .menu_group_title   = make_vec4(0.50f, 0.50f, 0.50f, 1.00f),

                    .applet_loaded      = make_vec4(0.14f, 0.75f, 0.55f, 1.00f),
                    .applet_unloaded    = make_vec4(0.85f, 0.45f, 0.25f, 1.00f),

                    .page_title         = make_vec4(0.85f, 0.45f, 0.25f, 1.00f),
                    .page_topic         = make_vec4(0.14f, 0.75f, 0.55f, 1.00f),
                },
            },
        },
        .apply_theme_cb = _on_apply_theme,
    };

    return &theme;
}
