////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui/cimgui.h>
#include <csr/graphics/ui/theme_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _on_apply_theme(struct ui_theme *theme)
{
    ImGuiStyle *style = igGetStyle();

    style->WindowRounding = 0.0f;
    style->WindowTitleAlign = (struct ImVec2){ 0.5f, 0.5f };
    style->WindowMenuButtonPosition = ImGuiDir_None;
    style->FrameRounding = 3.0f;
    style->FrameBorderSize = 0.0f;

    ////////////////////////////////////////

    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = (struct ImVec4) {0.75f, 0.75f, 0.75f, 1.00f};
    colors[ImGuiCol_TextDisabled]           = (struct ImVec4) {0.50f, 0.50f, 0.50f, 1.00f};
    colors[ImGuiCol_WindowBg]               = (struct ImVec4) {0.17f, 0.17f, 0.17f, 0.90f};
    colors[ImGuiCol_ChildBg]                = (struct ImVec4) {0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_PopupBg]                = (struct ImVec4) {0.17f, 0.17f, 0.17f, 0.90f};
    colors[ImGuiCol_Border]                 = (struct ImVec4) {0.43f, 0.43f, 0.50f, 0.50f};
    colors[ImGuiCol_BorderShadow]           = (struct ImVec4) {0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_FrameBg]                = (struct ImVec4) {0.28f, 0.30f, 0.33f, 0.54f};
    colors[ImGuiCol_FrameBgHovered]         = (struct ImVec4) {0.49f, 0.53f, 0.59f, 0.40f};
    colors[ImGuiCol_FrameBgActive]          = (struct ImVec4) {0.24f, 0.36f, 0.50f, 0.80f};
    colors[ImGuiCol_TitleBg]                = (struct ImVec4) {0.17f, 0.17f, 0.17f, 0.50f};
    colors[ImGuiCol_TitleBgActive]          = (struct ImVec4) {0.17f, 0.17f, 0.17f, 0.90f};
    colors[ImGuiCol_TitleBgCollapsed]       = (struct ImVec4) {0.17f, 0.17f, 0.17f, 0.90f};
    colors[ImGuiCol_MenuBarBg]              = (struct ImVec4) {0.17f, 0.17f, 0.17f, 0.90f};
    colors[ImGuiCol_ScrollbarBg]            = (struct ImVec4) {0.23f, 0.23f, 0.23f, 0.53f};
    colors[ImGuiCol_ScrollbarGrab]          = (struct ImVec4) {0.31f, 0.31f, 0.31f, 1.00f};
    colors[ImGuiCol_ScrollbarGrabHovered]   = (struct ImVec4) {0.41f, 0.41f, 0.41f, 1.00f};
    colors[ImGuiCol_ScrollbarGrabActive]    = (struct ImVec4) {0.51f, 0.51f, 0.51f, 1.00f};
    colors[ImGuiCol_CheckMark]              = (struct ImVec4) {0.45f, 0.59f, 0.75f, 1.00f};
    colors[ImGuiCol_SliderGrab]             = (struct ImVec4) {0.49f, 0.62f, 0.77f, 0.40f};
    colors[ImGuiCol_SliderGrabActive]       = (struct ImVec4) {0.49f, 0.62f, 0.77f, 0.40f};
    colors[ImGuiCol_Button]                 = (struct ImVec4) {0.49f, 0.62f, 0.77f, 0.40f};
    colors[ImGuiCol_ButtonHovered]          = (struct ImVec4) {0.24f, 0.36f, 0.50f, 0.80f};
    colors[ImGuiCol_ButtonActive]           = (struct ImVec4) {0.24f, 0.36f, 0.50f, 0.80f};
    colors[ImGuiCol_Header]                 = (struct ImVec4) {0.30f, 0.36f, 0.42f, 0.31f};
    colors[ImGuiCol_HeaderHovered]          = (struct ImVec4) {0.36f, 0.40f, 0.44f, 0.80f};
    colors[ImGuiCol_HeaderActive]           = (struct ImVec4) {0.24f, 0.36f, 0.50f, 0.80f};
    colors[ImGuiCol_Separator]              = (struct ImVec4) {0.43f, 0.43f, 0.50f, 0.50f};
    colors[ImGuiCol_SeparatorHovered]       = (struct ImVec4) {0.49f, 0.62f, 0.77f, 0.40f};
    colors[ImGuiCol_SeparatorActive]        = (struct ImVec4) {0.45f, 0.59f, 0.75f, 1.00f};
    colors[ImGuiCol_ResizeGrip]             = (struct ImVec4) {0.30f, 0.36f, 0.42f, 0.31f};
    colors[ImGuiCol_ResizeGripHovered]      = (struct ImVec4) {0.36f, 0.40f, 0.44f, 0.80f};
    colors[ImGuiCol_ResizeGripActive]       = (struct ImVec4) {0.24f, 0.36f, 0.50f, 0.80f};
    colors[ImGuiCol_Tab]                    = (struct ImVec4) {0.38f, 0.42f, 0.45f, 0.42f};
    colors[ImGuiCol_TabHovered]             = (struct ImVec4) {0.24f, 0.36f, 0.50f, 0.80f};
    colors[ImGuiCol_TabActive]              = (struct ImVec4) {0.26f, 0.31f, 0.38f, 1.00f};
    colors[ImGuiCol_TabUnfocused]           = (struct ImVec4) {0.38f, 0.42f, 0.45f, 0.42f};
    colors[ImGuiCol_TabUnfocusedActive]     = (struct ImVec4) {0.49f, 0.62f, 0.77f, 0.40f};
    colors[ImGuiCol_DockingPreview]         = (struct ImVec4) {0.26f, 0.59f, 0.98f, 0.70f};
    colors[ImGuiCol_DockingEmptyBg]         = (struct ImVec4) {0.20f, 0.20f, 0.20f, 1.00f};
    colors[ImGuiCol_PlotLines]              = (struct ImVec4) {0.61f, 0.61f, 0.61f, 1.00f};
    colors[ImGuiCol_PlotLinesHovered]       = (struct ImVec4) {1.00f, 0.43f, 0.35f, 1.00f};
    colors[ImGuiCol_PlotHistogram]          = (struct ImVec4) {0.90f, 0.70f, 0.00f, 1.00f};
    colors[ImGuiCol_PlotHistogramHovered]   = (struct ImVec4) {1.00f, 0.60f, 0.00f, 1.00f};
    colors[ImGuiCol_TableHeaderBg]          = (struct ImVec4) {0.19f, 0.19f, 0.20f, 1.00f};
    colors[ImGuiCol_TableBorderStrong]      = (struct ImVec4) {0.31f, 0.31f, 0.35f, 1.00f};
    colors[ImGuiCol_TableBorderLight]       = (struct ImVec4) {0.23f, 0.23f, 0.25f, 1.00f};
    colors[ImGuiCol_TableRowBg]             = (struct ImVec4) {0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_TableRowBgAlt]          = (struct ImVec4) {1.00f, 1.00f, 1.00f, 0.06f};
    colors[ImGuiCol_TextSelectedBg]         = (struct ImVec4) {0.24f, 0.36f, 0.50f, 0.80f};
    colors[ImGuiCol_DragDropTarget]         = (struct ImVec4) {1.00f, 1.00f, 0.00f, 0.90f};
    colors[ImGuiCol_NavHighlight]           = (struct ImVec4) {0.45f, 0.59f, 0.75f, 1.00f};
    colors[ImGuiCol_NavWindowingHighlight]  = (struct ImVec4) {1.00f, 1.00f, 1.00f, 0.70f};
    colors[ImGuiCol_NavWindowingDimBg]      = (struct ImVec4) {0.80f, 0.80f, 0.80f, 0.20f};
    colors[ImGuiCol_ModalWindowDimBg]       = (struct ImVec4) {0.80f, 0.80f, 0.80f, 0.35f};
}

struct ui_theme* ui_theme_csr_graphite_ptr()
{
    static struct ui_theme theme = {
        .info = {
            .name = "CSR Graphite",
            .author = "Andreas Gummer",
            .version = make_version(1, 0, 0),

            .style = {
                .color = {
                    .background = make_vec4(0.22, 0.22, 0.22, 1.0),

                    .log_info = make_vec4(0.75f, 0.75f, 0.75f, 1.00f),
                    .log_notice = make_vec4(0.44f, 0.85f, 0.85f, 1.00f),
                    .log_warn = make_vec4(1.00f, 0.60f, 0.40f, 1.00f),
                    .log_error = make_vec4(1.00f, 0.43f, 0.35f, 1.00f),
                    .log_trace = make_vec4(0.74f, 0.50f, 0.73f, 1.00f),
                },
            },
        },
        .apply_theme_cb = _on_apply_theme,
    };

    return &theme;
}
