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

            .style = {
                .color.background = make_vec4(0.45, 0.55, 0.60, 1.0),
            },
        },
        .apply_theme_cb = _on_apply_theme,
    };

    return &theme;
}
