////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/list.h>

#include "workspace.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_workspace
{
    struct string name;

    bool enable_docking;

    void (*workspace_tick_cb)(struct ui_workspace *workspace);

    struct list *contexts;

    struct {
        struct ui_window *focused_window;
    } state;
};

void ui_workspace_tick(struct ui_workspace *workspace);
