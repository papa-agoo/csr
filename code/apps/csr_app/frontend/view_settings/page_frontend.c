////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// user interface
////////////////////////////////////////////////////////////////////////////////////////////////////

// content scale
////////////////////////////////////////////////////////////
static bool _cond_use_custom_content_scale()
{
    return application_conf_ptr()->ui.content_scale.use_custom_scale;
}

static void _draw_ui_override_content_scale()
{
    igCheckbox("##use_custom_content_scale", &application_conf_ptr()->ui.content_scale.use_custom_scale);
}

static void _draw_ui_content_scale()
{
    struct ui_conf *conf = &application_conf_ptr()->ui;

    ////////////////////////////////////////

    f32 scale = 100.0 * ui_get_content_scale();

    f32 scale_min = 100.0 * UI_CONTENT_SCALE_MIN;
    f32 scale_max = 100.0 * UI_CONTENT_SCALE_MAX;

    ////////////////////////////////////////

    ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;

    if (!conf->content_scale.use_custom_scale) {
        flags = ImGuiSliderFlags_ReadOnly | ImGuiSliderFlags_NoInput;
    }

    ////////////////////////////////////////

    if (igDragFloat("##content_scale_factor", &scale, 1, scale_min, scale_max, "%.0f%%", flags)) {
        ui_set_content_scale(scale / 100.0);
    }
}


// theme
////////////////////////////////////////////////////////////
static void _draw_ui_color_scheme()
{
    // FIXME ui_get_theme_list()
    const char *themes_list[] = { "ImGui Dark", "CSR Graphite" };
    u32 theme_count = COUNT_OF(themes_list);

    u32 selection = ui_get_theme();

    if (igCombo_Str_arr("##color_scheme", &selection, themes_list, theme_count, 3))
    {
        ui_set_theme((enum ui_theme_type)selection);
    }
}


// fonts
////////////////////////////////////////////////////////////
static void _draw_ui_current_font()
{
    // FIXME ui_get_font_info()
    struct ui_font_info info = {0};
    info.size = 13;
    info.uri = "ProggyClean.ttf";

    igText("%s, %.0fpt", info.uri, info.size);
}

static void _draw_ui_settings()
{
    // content scale set
    struct ui_property properties_content_scale[] = {
        {
            .name = "Custom Scale",
            .draw_cb = _draw_ui_override_content_scale,
        },
        {
            .draw_cb = _draw_ui_content_scale,
            .draw_cond_cb = _cond_use_custom_content_scale,
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
            .name = "Font Face",
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
        &set_content_scale,
        &set_theme,
        &set_font,
    };

    struct ui_property_set_group group = {0};
    group.name = "User Interface";
    group.sets = sets;
    group.set_count = COUNT_OF(sets);

    ui_property_set_group_draw(&group);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

void _draw_frontend_settings_page(struct ui_view* view)
{
    _draw_ui_settings();
}
