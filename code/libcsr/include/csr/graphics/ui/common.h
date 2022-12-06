////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "imgui.h"
#include "theme.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define UI_CONTENT_SCALE_FACTOR_MIN 1.0
#define UI_CONTENT_SCALE_FACTOR_MAX 4.0


////////////////////////////////////////////////////////////////////////////////////////////////////
// fonts
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ui_font_info
{
    const char *name;

    f32 size;
    f32 size_scaled;
};

struct ui_font
{
    struct ui_font_info info;

    void *data;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// menus
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ui_menu
{
    const char *key;

    const char *title;

    void (*draw_cb)(struct ui_menu* menu, struct ui_style *style);
    bool (*is_enabled_cb)(struct ui_menu *menu);
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// windows, views
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ui_view
{
    const char* name;

    void (*draw_cb)(struct ui_view* view, struct ui_style *style);

    struct ui_window* parent;

    void* user_data;
};

struct ui_window
{
    const char* key;

    const char* title;

    bool is_viewport;
    bool is_opened;

    ImGuiWindowFlags flags;

    void (*push_properties_cb)(struct ui_window* window);
    void (*pop_properties_cb)(struct ui_window* window);

    struct ui_view view;

    void* user_data;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////

// page layout
// 
// - property page
//      - property set group (collapsable header)
//          - property set (section of the group)
//              - property
//              - property
//              - ...

struct ui_property
{
    const char *name;
    const char *tooltip;

    void (*draw_cb)();
    bool (*draw_cond_cb)();

    bool is_hidden;
    // bool is_disabled;
};

struct ui_property_set
{
    const char *name;

    u32 property_count;
    struct ui_property *properties;
};

struct ui_property_set_group
{
    const char *name;

    u32 set_count;
    struct ui_property_set **sets;
};

struct ui_property_page
{
    const char *title;

    void (*draw_cb)(struct ui_view *view, struct ui_style *style);
};
