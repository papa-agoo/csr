////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>
#include <csr/core/math/vec4.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum ui_theme_type
{
    UI_THEME_TYPE_IMGUI_DARK,
    UI_THEME_TYPE_CSR_GRAPHITE,
};

struct ui_style
{
    struct {
        struct vec4 background;

        struct vec4 log_info;
        struct vec4 log_notice;
        struct vec4 log_warn;
        struct vec4 log_error;
        struct vec4 log_trace;

        struct vec4 menu_head_on;
        struct vec4 menu_head_off;
        struct vec4 menu_group_title;

        struct vec4 applet_loaded;
        struct vec4 applet_unloaded;

        struct vec4 page_title;
        struct vec4 page_topic;
    } color;
};

struct ui_theme_info
{
    struct string name;
    struct string author;
    struct version version;

    enum ui_theme_type theme_type;
    struct ui_style style;
};
