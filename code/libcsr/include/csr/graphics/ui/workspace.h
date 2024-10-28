////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ctx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_workspace;

struct ui_workspace_create_info
{
    struct string name;

    bool enable_docking;

    void (*workspace_tick_cb)(struct ui_workspace *workspace);
};

struct ui_workspace* ui_workspace_create(struct ui_workspace_create_info *info);
void ui_workspace_destroy(struct ui_workspace* workspace);

struct string ui_workspace_get_name(struct ui_workspace* workspace);

// contexts
result_e ui_workspace_add_context(struct ui_workspace *workspace, struct ui_ctx* ctx);
void ui_workspace_remove_context(struct ui_workspace *workspace, struct ui_ctx* ctx);

typedef void (*ui_workspace_traverse_contexts_cb_t)(struct ui_ctx *ctx, void *data);
void ui_workspace_traverse_contexts(struct ui_workspace *workspace, ui_workspace_traverse_contexts_cb_t callback, void *data);

// windows
void ui_workspace_set_focused_window(struct ui_workspace *workspace, struct ui_window *window);
struct ui_window* ui_workspace_get_focused_window(struct ui_workspace *workspace);
