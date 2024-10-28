////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/ui_priv.h>
#include <csr/graphics/ui/theme_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

extern struct ui_theme* ui_theme_imgui_dark_ptr();
extern struct ui_theme* ui_theme_csr_graphite_ptr();

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _apply_style_sizes(ImGuiStyle *style)
{
    // ScaleAllSizes() does relative scaling, so we need to normalize sizes first
    {
        // main
        style->WindowPadding            = make_ImVec2( 8.0f, 8.0f );
        style->WindowMinSize            = make_ImVec2(32.0f,32.0f );
        style->FramePadding             = make_ImVec2( 6.0f, 3.0f );
        style->CellPadding              = make_ImVec2( 4.0f, 4.0f );
        style->ItemSpacing              = make_ImVec2( 8.0f, 4.0f );
        style->ItemInnerSpacing         = make_ImVec2( 4.0f, 4.0f );
        style->TouchExtraPadding        = make_ImVec2( 0.0f, 0.0f );
        style->DisplayWindowPadding     = make_ImVec2(19.0f,19.0f );
        style->IndentSpacing            = 21.0f;
        style->ScrollbarSize            = 14.0f;
        style->GrabMinSize              = 10.0f;
        style->ColumnsMinSpacing        = 8.0f;
        style->TabMinWidthForCloseButton = 0.0f;
        style->MouseCursorScale         = 1.0f;

        // borders
        style->WindowBorderSize         = 1.0f;
        style->ChildBorderSize          = 1.0f;
        style->PopupBorderSize          = 1.0f;
        style->FrameBorderSize          = 0.0f;
        style->TabBorderSize            = 0.0f;

        // rounding
        style->WindowRounding           = 0.0f;
        style->ChildRounding            = 0.0f;
        style->FrameRounding            = 0.0f;
        style->PopupRounding            = 0.0f;
        style->ScrollbarRounding        = 9.0f;
        style->GrabRounding             = 0.0f;
        style->LogSliderDeadzone        = 4.0f;
        style->TabRounding              = 4.0f;

        // alignment
        style->WindowTitleAlign         = make_ImVec2( 0.0f, 0.5f );
        style->WindowMenuButtonPosition = ImGuiDir_Left;
        style->ColorButtonPosition      = ImGuiDir_Right;
        style->ButtonTextAlign          = make_ImVec2( 0.5f, 0.5f );
        style->SelectableTextAlign      = make_ImVec2( 0.0f, 0.0f );

        // safe area padding
        style->DisplaySafeAreaPadding   = make_ImVec2( 3.0f, 3.0f );
    }

    // update global style sizes
    ImGuiStyle_ScaleAllSizes(style, ui_get_content_scale_factor());
}

static struct ui_theme* _lookup_theme(enum ui_theme_type theme)
{
    switch (theme)
    {
        case UI_THEME_TYPE_IMGUI_DARK:
            return ui_theme_imgui_dark_ptr();

        case UI_THEME_TYPE_CSR_GRAPHITE:
            return ui_theme_csr_graphite_ptr();

        default:
            return NULL;
    }
}

void ui_set_theme(enum ui_theme_type theme_type)
{
    csr_assert(ui_ptr()->is_initialized);

    struct ui_theme *theme = _lookup_theme(theme_type);

    check_ptr(theme);
    check_ptr(theme->apply_theme_cb);

    _apply_style_sizes(igGetStyle());

    theme->apply_theme_cb(theme);

    ui_conf_ptr()->theme = theme_type;

error:
    return;
}

enum ui_theme_type ui_get_theme()
{
    csr_assert(ui_ptr()->is_initialized);

    return ui_conf_ptr()->theme;
}

const struct ui_style* ui_get_theme_style()
{
    const struct ui_theme_info *theme_info = ui_get_theme_info();
    check_ptr(theme_info);

    return &theme_info->style;

error:
    return NULL;
}

const struct ui_theme_info* ui_get_theme_info()
{
    csr_assert(ui_ptr()->is_initialized);

    struct ui_theme *theme = _lookup_theme( ui_ptr()->conf->theme );
    check_ptr(theme);

    return &theme->info;

error:
    return NULL;
}
