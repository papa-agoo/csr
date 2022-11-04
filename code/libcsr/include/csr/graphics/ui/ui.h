////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "imgui.h"
#include "theme.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define UI_CONTENT_SCALE_MIN 0.5
#define UI_CONTENT_SCALE_MAX 4.0

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_conf
{
    struct {
        bool use_custom_scale;

        f32 scale_factor;
    } content_scale;

    struct {
        bool use_custom_font;

        const char *name;
        f32 size;
    } font;

    enum ui_theme_type theme;
};

static void ui_conf_defaults(struct ui_conf *conf)
{
    check_ptr(conf);

    // content scale
    conf->content_scale.use_custom_scale = false;
    conf->content_scale.scale_factor = 1.0f;

    // custom font
    conf->font.use_custom_font = false;
    conf->font.name = "Consolas.ttf";
    conf->font.size = 14.0;

    // theme
    conf->theme = UI_THEME_TYPE_CSR_GRAPHITE;

error:
    return;
}

////////////////////////////////////////////////////////////

struct ui_init_info
{
    const char *imgui_ini_file;

    void (*ui_tick_cb)();

    struct ui_conf *conf;
};

result_e ui_init(struct ui_init_info *init_info);
void ui_quit();

void ui_tick();

struct ui_conf* ui_get_config();

// themes
void ui_set_theme(enum ui_theme_type theme);
enum ui_theme_type ui_get_theme();
const struct ui_theme_info* ui_get_theme_info(enum ui_theme_type theme);

typedef void (*ui_traverse_themes_cb_t)(const struct ui_theme_info *info);
void ui_traverse_themes(ui_traverse_themes_cb_t callback);

// content scale
f32 ui_get_content_scale();
void ui_set_content_scale(f32 scale);
