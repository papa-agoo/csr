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

        string_cstr name; // FIXME
        f32 size;
    } font;

    enum ui_theme_type theme;
};

static void ui_conf_defaults(struct ui_conf *conf)
{
    check_ptr(conf);

    // content
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
    struct string imgui_ini_file;
    struct string fonts_dir;

    struct ui_workspace *workspace;

    struct ui_conf *conf;
};

result_e ui_init(struct ui_init_info *init_info);
void ui_quit();

void ui_tick();

struct ui_conf* ui_get_config();

void ui_toggle_imgui_demo_window();

// themes
void ui_set_theme(enum ui_theme_type theme);
enum ui_theme_type ui_get_theme();

const struct ui_style* ui_get_theme_style();
const struct ui_theme_info* ui_get_theme_info();

// fonts
struct ui_font_info ui_get_font_info();

// content scale
s32 ui_get_content_scale();
void ui_set_content_scale(s32 scale);

f32 ui_get_content_scale_factor();
void ui_set_content_scale_factor(f32 factor);

// workspaces
// result_e ui_add_workspace(struct ui_workspace *workspace);
// void ui_remove_workspace(struct ui_workspace *workspace);
struct ui_workspace* ui_get_workspace();

// typedef void (*ui_traverse_workspaces_cb_t)(struct ui_workspace *workspace, void *data);
// void ui_traverse_workspaces(ui_traverse_workspaces_cb_t callback, void *data);

// menus
void ui_menu_init(struct ui_menu *menu, string_cstr title);
void ui_menu_draw(struct ui_menu* menu, struct ui_style *style);

// windows / views
void ui_view_init(struct ui_view *view, enum ui_view_type view_type, void* user_data);

void ui_window_init(struct ui_window *window, string_cstr title);
void ui_window_draw(struct ui_window* window, struct ui_style *style);

struct ui_window* ui_window_get_focused();
void ui_window_set_focused(struct ui_window* window);
bool ui_window_is_focused(struct ui_window* window);

// properties
void ui_property_draw(struct ui_property *property, struct ui_style *style);
void ui_property_set_draw(struct ui_property_set *set, struct ui_style *style);
void ui_property_set_group_draw(struct ui_property_set_group *group, struct ui_style *style);
