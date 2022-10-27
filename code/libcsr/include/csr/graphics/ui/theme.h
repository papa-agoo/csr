////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/math/vec4.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum ui_theme_type
{
    UI_THEME_TYPE_UNKNOWN,

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
    } color;
};

struct ui_theme_info
{
    const char *name;
    const char *author;
    struct version version;

    enum ui_theme_type theme_type;
    struct ui_style style;
};