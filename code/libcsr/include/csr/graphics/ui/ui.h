////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "imgui.h"
#include "theme.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_conf
{
    enum ui_theme_type theme;
};

static void ui_conf_defaults(struct ui_conf *conf)
{
    check_ptr(conf);

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

// themes
void ui_set_theme(enum ui_theme_type theme);
enum ui_theme_type ui_get_theme();
const struct ui_theme_info* ui_get_theme_info(enum ui_theme_type theme);

typedef void (*ui_traverse_themes_cb_t)(const struct ui_theme_info *info);
void ui_traverse_themes(ui_traverse_themes_cb_t callback);
