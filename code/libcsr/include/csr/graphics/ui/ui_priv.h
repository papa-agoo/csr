////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ui.h"
#include "workspace_priv.h"

#include "imgui/cimgui_priv.h"

#define KLOG_MODULE_NAME ui
#include <csr/kernel/kio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui
{
    bool is_initialized;
    bool show_imgui_demo_window;
    bool update_content_scale;

    struct ui_workspace *workspace;

    struct ui_conf *conf;

    struct {
        struct ui_font custom;
        struct ui_font builtin;
    } font;
    
    const char *fonts_dir;
};

struct ui* ui_ptr();

#define ui_conf_ptr() (ui_ptr()->conf)

void ui_build_fonts();
