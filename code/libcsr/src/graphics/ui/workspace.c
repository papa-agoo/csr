////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/workspace_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_workspace* ui_workspace_create(struct ui_workspace_create_info *info)
{
    check_ptr(info);
    check_ptr(info->workspace_tick_cb);

    struct ui_workspace *workspace = calloc(1, sizeof(struct ui_workspace));
    check_mem(workspace);

    workspace->name = string_is_valid(info->name) ? info->name : make_string("<unnamed workspace>");
    workspace->enable_docking = info->enable_docking;
    workspace->workspace_tick_cb = info->workspace_tick_cb;

    workspace->contexts = list_create();
    check_ptr(workspace->contexts);

    return workspace;

error:
    return NULL;
}

static void _free_list_element_cb(void *ctx)
{
    ui_ctx_destroy(ctx);
}

void ui_workspace_destroy(struct ui_workspace* workspace)
{
    check_ptr(workspace);

    list_destroy(workspace->contexts, _free_list_element_cb);

error:
    return;
}

struct string ui_workspace_get_name(struct ui_workspace* workspace)
{
    check_ptr(workspace);

    return workspace->name;

error:
    return make_string_invalid();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// contexts
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e ui_workspace_add_context(struct ui_workspace *workspace, struct ui_ctx* ctx)
{
    check_ptr(workspace);
    check_ptr(ctx);

    list_push_back(workspace->contexts, ctx);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void ui_workspace_remove_context(struct ui_workspace *workspace, struct ui_ctx* ctx)
{
    check_ptr(workspace);
    check_ptr(ctx);

    list_remove_by_value(workspace->contexts, ctx);

    ui_workspace_set_focused_window(workspace, NULL);

error:
    return;
}

void ui_workspace_traverse_contexts(struct ui_workspace *workspace, ui_workspace_traverse_contexts_cb_t callback, void *data)
{
    check_ptr(workspace);
    check_ptr(callback);

    LIST_FOREACH(workspace->contexts, first, next, current) {
        callback(current->value, data);
    }

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// windows
////////////////////////////////////////////////////////////////////////////////////////////////////
void ui_workspace_set_focused_window(struct ui_workspace *workspace, struct ui_window *window)
{
    check_ptr(workspace);

    // NULL means no window is currently focused
    workspace->state.focused_window = window;

error:
    return;
}

struct ui_window* ui_workspace_get_focused_window(struct ui_workspace *workspace)
{
    check_ptr(workspace);

    return workspace->state.focused_window;

error:
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// priv
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _draw_dock_space()
{
    ImGuiViewport *viewport = igGetMainViewport();
    igSetNextWindowPos(viewport->WorkPos, 0, make_ImVec2_zero());
    igSetNextWindowSize(viewport->WorkSize, 0);
    igSetNextWindowViewport(viewport->ID);

    igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0f);
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0f);
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, make_ImVec2_zero());

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    window_flags |= ImGuiWindowFlags_NoNavFocus;

    igBegin("DockSpace", NULL, window_flags);
    {
        igPopStyleVar(3);

        ImGuiDockNodeFlags dock_flags = 0;
        dock_flags |= ImGuiDockNodeFlags_AutoHideTabBar;

        ImGuiID dockspace_id = igGetID_Str("MainDockSpace");
        igDockSpace(dockspace_id, make_ImVec2_zero(), dock_flags, NULL);

        igEnd();
    }
}

void ui_workspace_tick(struct ui_workspace *workspace)
{
    if (!workspace || (workspace && !workspace->workspace_tick_cb))
    {
        static bool show_demo_win = true;

        igCheckbox("Show ImGui Demo Window", &show_demo_win);

        if (show_demo_win) {
            igShowDemoWindow(&show_demo_win);
        }

        return;
    }

    ////////////////////////////////////////

    if (workspace->enable_docking) {
        _draw_dock_space();
    }

    workspace->workspace_tick_cb(workspace);
}
