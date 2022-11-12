////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"
#include "workspace.h"

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

    struct ui_workspace *workspace;

    struct ui_conf *conf;
};

result_e ui_init(struct ui_init_info *init_info);
void ui_quit();

void ui_tick();

struct ui_conf* ui_get_config();

// themes
void ui_set_theme(enum ui_theme_type theme);
enum ui_theme_type ui_get_theme();

const struct ui_style* ui_get_theme_style();
const struct ui_theme_info* ui_get_theme_info();

typedef void (*ui_traverse_themes_cb_t)(const struct ui_theme_info *info);
void ui_traverse_themes(ui_traverse_themes_cb_t callback);

// content scale
f32 ui_get_content_scale();
void ui_set_content_scale(f32 scale);

// workspaces
// result_e ui_add_workspace(struct ui_workspace *workspace);
// void ui_remove_workspace(struct ui_workspace *workspace);
struct ui_workspace* ui_get_workspace();

// typedef void (*ui_traverse_workspaces_cb_t)(struct ui_workspace *workspace, void *data);
// void ui_traverse_workspaces(ui_traverse_workspaces_cb_t callback, void *data);

// menus
void ui_menu_init(struct ui_menu *menu, const char *title);
void ui_menu_draw(struct ui_menu* menu, struct ui_style *style);

// windows
typedef void (*ui_window_init_view_cb_t)(struct ui_view *view);

void ui_window_init(struct ui_window *window, const char *title, ui_window_init_view_cb_t callback);
void ui_window_draw(struct ui_window* window, struct ui_style *style);

// properties
void ui_property_draw(struct ui_property *property);
void ui_property_set_draw(struct ui_property_set *set);
void ui_property_set_group_draw(struct ui_property_set_group *group);
