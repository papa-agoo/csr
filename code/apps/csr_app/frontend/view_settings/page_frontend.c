////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// user interface
////////////////////////////////////////////////////////////////////////////////////////////////////

// content scale
////////////////////////////////////////////////////////////
static void _draw_ui_override_content_scale()
{
    if (igCheckbox("##use_custom_content_scale", &application_conf_ptr()->ui.content_scale.use_custom_scale)) {
        ui_set_content_scale_factor(ui_get_content_scale_factor());
    }
}

static void _draw_ui_content_scale()
{
    struct ui_conf *conf = &application_conf_ptr()->ui;

    ////////////////////////////////////////

    f32 scale = ui_get_content_scale();

    f32 scale_min = UI_CONTENT_SCALE_FACTOR_MIN * 100.0;
    f32 scale_max = UI_CONTENT_SCALE_FACTOR_MAX * 100.0;

    ////////////////////////////////////////

    ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;

    if (!conf->content_scale.use_custom_scale) {
        flags = ImGuiSliderFlags_ReadOnly | ImGuiSliderFlags_NoInput;
    }

    ////////////////////////////////////////

    if (igDragFloat("##content_scale_factor", &scale, 1, scale_min, scale_max, "%.0f%%", flags)) {
        ui_set_content_scale(scale);
    }
}


// theme
////////////////////////////////////////////////////////////
static void _draw_ui_color_scheme()
{
    // FIXME ui_get_theme_list()
    string_cstr themes_list[] = { "ImGui Dark", "CSR Graphite" };
    u32 theme_count = COUNT_OF(themes_list);

    s32 selection = (s32)ui_get_theme();

    if (igCombo_Str_arr("##color_scheme", &selection, themes_list, theme_count, 3))
    {
        ui_set_theme((enum ui_theme_type)selection);
    }
}


// fonts
////////////////////////////////////////////////////////////
static void _draw_ui_override_font()
{
    if (igCheckbox("##use_custom_font", &application_conf_ptr()->ui.font.use_custom_font))
    {
        // FIXME event : content_scale_changed
        ui_set_content_scale(ui_get_content_scale());
    }
}

static void _draw_ui_current_font()
{
    struct ui_font_info info = ui_get_font_info();

    igText("%s, %.0fpt (%.0fpt @ %d%%)", info.name, info.size, info.size_scaled, ui_get_content_scale());
}

static void _draw_ui_settings(struct ui_style *style)
{
    // content scale set
    struct ui_property properties_content_scale[] = {
        {
            .name = "Custom Scale",
            .tooltip = "Override recommended Content Scale",
            .draw_cb = _draw_ui_override_content_scale,
        },
        {
            .draw_cb = _draw_ui_content_scale,
        },
    };

    struct ui_property_set set_content_scale = {0};
    set_content_scale.name = "Content Scale";
    set_content_scale.properties = properties_content_scale;
    set_content_scale.property_count = COUNT_OF(properties_content_scale);

    ////////////////////////////////////////

    // font set
    struct ui_property properties_font[] = {
        {
            .name = "Custom Font",
            .tooltip = "Override builtin Font (ProggyClean.ttf, 13pt)",
            .draw_cb = _draw_ui_override_font,
        },
        {
            .draw_cb = _draw_ui_current_font,
        },
    };

    struct ui_property_set set_font = {0};
    set_font.name = "Fonts";
    set_font.properties = properties_font;
    set_font.property_count = COUNT_OF(properties_font);

    ////////////////////////////////////////

    // theme set
    struct ui_property properties_theme[] = {
        {
            .name = "Color Scheme",
            .draw_cb = _draw_ui_color_scheme,
        },
    };

    struct ui_property_set set_theme = {0};
    set_theme.name = "Theme";
    set_theme.properties = properties_theme;
    set_theme.property_count = COUNT_OF(properties_theme);

    ////////////////////////////////////////

    // draw sets
    struct ui_property_set* sets[] = {
        &set_theme,
        &set_content_scale,
        &set_font,
    };

    struct ui_property_set_group group = {0};
    group.name = "User Interface";
    group.sets = sets;
    group.set_count = COUNT_OF(sets);

    ui_property_set_group_draw(&group, style);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void _draw_frontend_settings_page(struct ui_view* view, struct ui_style *style)
{
    _draw_ui_settings(style);
}
