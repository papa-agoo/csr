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

    struct ui_workspace *workspace;

    struct ui_conf *conf;
};

struct ui* ui_ptr();

#define ui_conf_ptr() (ui_ptr()->conf)
