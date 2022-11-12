////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui/cimgui.h>
#include <csr/graphics/ui/theme_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _on_apply_theme(struct ui_theme *theme)
{
    ImGuiStyle *style = igGetStyle();

    style->WindowRounding = 0.0f;
    style->WindowTitleAlign = make_ImVec2( 0.5f, 0.5f );
    style->WindowMenuButtonPosition = ImGuiDir_None;
    style->FrameRounding = 3.0f;
    style->FrameBorderSize = 0.0f;

    ////////////////////////////////////////

    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = make_ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = make_ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = make_ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_ChildBg]                = make_ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = make_ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_Border]                 = make_ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = make_ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = make_ImVec4(0.28f, 0.30f, 0.33f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = make_ImVec4(0.49f, 0.53f, 0.59f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = make_ImVec4(0.24f, 0.36f, 0.50f, 0.80f);
    colors[ImGuiCol_TitleBg]                = make_ImVec4(0.17f, 0.17f, 0.17f, 0.50f);
    colors[ImGuiCol_TitleBgActive]          = make_ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_TitleBgCollapsed]       = make_ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_MenuBarBg]              = make_ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_ScrollbarBg]            = make_ImVec4(0.23f, 0.23f, 0.23f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = make_ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = make_ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = make_ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = make_ImVec4(0.45f, 0.59f, 0.75f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = make_ImVec4(0.49f, 0.62f, 0.77f, 0.40f);
    colors[ImGuiCol_SliderGrabActive]       = make_ImVec4(0.49f, 0.62f, 0.77f, 0.40f);
    colors[ImGuiCol_Button]                 = make_ImVec4(0.49f, 0.62f, 0.77f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = make_ImVec4(0.24f, 0.36f, 0.50f, 0.80f);
    colors[ImGuiCol_ButtonActive]           = make_ImVec4(0.24f, 0.36f, 0.50f, 0.80f);
    colors[ImGuiCol_Header]                 = make_ImVec4(0.30f, 0.36f, 0.42f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = make_ImVec4(0.36f, 0.40f, 0.44f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = make_ImVec4(0.24f, 0.36f, 0.50f, 0.80f);
    colors[ImGuiCol_Separator]              = make_ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = make_ImVec4(0.49f, 0.62f, 0.77f, 0.40f);
    colors[ImGuiCol_SeparatorActive]        = make_ImVec4(0.45f, 0.59f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = make_ImVec4(0.30f, 0.36f, 0.42f, 0.31f);
    colors[ImGuiCol_ResizeGripHovered]      = make_ImVec4(0.36f, 0.40f, 0.44f, 0.80f);
    colors[ImGuiCol_ResizeGripActive]       = make_ImVec4(0.24f, 0.36f, 0.50f, 0.80f);
    colors[ImGuiCol_Tab]                    = make_ImVec4(0.38f, 0.42f, 0.45f, 0.42f);
    colors[ImGuiCol_TabHovered]             = make_ImVec4(0.24f, 0.36f, 0.50f, 0.80f);
    colors[ImGuiCol_TabActive]              = make_ImVec4(0.26f, 0.31f, 0.38f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = make_ImVec4(0.38f, 0.42f, 0.45f, 0.42f);
    colors[ImGuiCol_TabUnfocusedActive]     = make_ImVec4(0.49f, 0.62f, 0.77f, 0.40f);
    colors[ImGuiCol_DockingPreview]         = make_ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg]         = make_ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = make_ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = make_ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = make_ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = make_ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = make_ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = make_ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = make_ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = make_ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = make_ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = make_ImVec4(0.24f, 0.36f, 0.50f, 0.80f);
    colors[ImGuiCol_DragDropTarget]         = make_ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = make_ImVec4(0.45f, 0.59f, 0.75f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = make_ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = make_ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = make_ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
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
                    .background         = make_vec4(0.22, 0.22, 0.22, 1.0),

                    .log_info           = make_vec4(0.75f, 0.75f, 0.75f, 1.00f),
                    .log_notice         = make_vec4(0.44f, 0.85f, 0.85f, 1.00f),
                    .log_warn           = make_vec4(1.00f, 0.60f, 0.40f, 1.00f),
                    .log_error          = make_vec4(1.00f, 0.43f, 0.35f, 1.00f),
                    .log_trace          = make_vec4(0.74f, 0.50f, 0.73f, 1.00f),

                    .menu_head_on       = make_vec4(0.14f, 0.75f, 0.55f, 1.00f),
                    .menu_head_off      = make_vec4(0.55f, 0.55f, 0.55f, 1.00f),

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
