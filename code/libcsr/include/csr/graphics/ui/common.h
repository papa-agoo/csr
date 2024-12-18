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
    string_cstr name;

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
    string_cstr key;
    string_cstr title;

    void (*draw_cb)(struct ui_menu* menu, struct ui_style *style);
    bool (*draw_cond_cb)(struct ui_menu *menu);
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// windows, views
////////////////////////////////////////////////////////////////////////////////////////////////////
enum ui_view_type
{
    UI_VIEW_TYPE_UNKNOWN,

    UI_VIEW_TYPE_CUSTOM,
    UI_VIEW_TYPE_SCREEN,
};

struct ui_view
{
    enum ui_view_type type;

    void (*draw_cb)(struct ui_view* view, struct ui_style *style);

    void* parent;
    void* user_data;
};

struct ui_window
{
    string_cstr  key;
    string_cstr title;

    bool is_opened;

    struct ui_view view;

    struct {
        ImGuiWindowFlags flags;

        void (*push_properties_cb)(struct ui_window* window);
        void (*pop_properties_cb)(struct ui_window* window);

        // initial window setup (no ini available)
        struct vec2 size;
        struct vec2 position;

        // needed for screens
        struct vec2 content_size;
    } priv;

    struct {
        bool show_surface_info;
        bool fit_window_to_content_size;
    } hint;
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
    string_cstr name;
    string_cstr tooltip;

    void (*draw_cb)();
    bool (*draw_cond_cb)();

    bool is_hidden;
    // bool is_disabled;
};

struct ui_property_set
{
    string_cstr name;

    u32 property_count;
    struct ui_property *properties;
};

struct ui_property_set_group
{
    string_cstr name;

    u32 set_count;
    struct ui_property_set **sets;
};

struct ui_property_page
{
    string_cstr title;

    void (*draw_cb)(struct ui_view *view, struct ui_style *style);
};
