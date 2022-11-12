////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/list.h>

#include "workspace.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_workspace
{
    const char *name;

    bool enable_docking;

    void (*workspace_tick_cb)(struct ui_workspace *workspace);

    struct list *contexts;
};

void ui_workspace_tick(struct ui_workspace *workspace);
