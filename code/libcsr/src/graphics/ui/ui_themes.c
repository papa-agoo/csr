////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/ui_priv.h>
#include <csr/graphics/ui/theme_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

extern struct ui_theme* ui_theme_imgui_dark_ptr();
extern struct ui_theme* ui_theme_csr_graphite_ptr();

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _apply_imgui_style_defaults()
{
    ImGuiStyle *style = igGetStyle();

    // main
    style->WindowPadding            = (struct ImVec2) { 8.0f, 8.0f };
    style->FramePadding             = (struct ImVec2) { 6.0f, 3.0f };
    style->CellPadding              = (struct ImVec2) { 4.0f, 4.0f };
    style->ItemSpacing              = (struct ImVec2) { 8.0f, 4.0f };
    style->ItemInnerSpacing         = (struct ImVec2) { 4.0f, 4.0f };
    style->TouchExtraPadding        = (struct ImVec2) { 0.0f, 0.0f };
    style->IndentSpacing            = 21.0f;
    style->ScrollbarSize            = 14.0f;
    style->GrabMinSize              = 10.0f;

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
    style->WindowTitleAlign         = (struct ImVec2) { 0.0f, 0.5f };
    style->WindowMenuButtonPosition = ImGuiDir_Left;
    style->ColorButtonPosition      = ImGuiDir_Right;
    style->ButtonTextAlign          = (struct ImVec2) { 0.5f, 0.5f };
    style->SelectableTextAlign      = (struct ImVec2) { 0.0f, 0.0f };

    // safe area padding
    style->DisplaySafeAreaPadding   = (struct ImVec2) { 3.0f, 3.0f };
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

    check_expr(theme_type != UI_THEME_TYPE_UNKNOWN);

    struct ui_theme *theme = _lookup_theme(theme_type);
    check_ptr(theme);
    check_ptr(theme->apply_theme_cb);

    klog_info("setting theme (%s) ...", theme->info.name);

    _apply_imgui_style_defaults();

    theme->apply_theme_cb(theme);

    ui_conf_ptr()->theme = theme_type;

error:
    return;
}

enum ui_theme_type ui_get_theme()
{
    csr_assert(ui_ptr()->is_initialized);

    return ui_conf_ptr()->theme;

error:
    return UI_THEME_TYPE_UNKNOWN;
}

const struct ui_theme_info* ui_get_theme_info(enum ui_theme_type theme_type)
{
    csr_assert(ui_ptr()->is_initialized);

    check_expr(theme_type != UI_THEME_TYPE_UNKNOWN);

    struct ui_theme *theme = _lookup_theme(theme_type);
    check_ptr(theme);

    return &theme->info;

error:
    return NULL;
}

void ui_traverse_themes(ui_traverse_themes_cb_t callback)
{
    check_ptr(callback);

    callback(ui_get_theme_info(UI_THEME_TYPE_IMGUI_DARK));
    callback(ui_get_theme_info(UI_THEME_TYPE_CSR_GRAPHITE));

error:
    return;
}