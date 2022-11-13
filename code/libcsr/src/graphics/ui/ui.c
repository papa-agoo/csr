////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/ui_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct ui g_ui;

struct ui* ui_ptr()
{
    return &g_ui;
}

result_e ui_init(struct ui_init_info *init_info)
{
    csr_assert(!ui_ptr()->is_initialized);

    check_ptr(init_info);
    check_ptr(init_info->imgui_ini_file);
    check_ptr(init_info->conf);

    ui_ptr()->conf = init_info->conf;
    ui_ptr()->workspace = init_info->workspace;

    ////////////////////////////////////////

    // init imgui
    klog_notice("initializing ui ...");

    result_e result = cimgui_init(init_info->imgui_ini_file);
    check(R_SUCCESS(result), "cimgui_init() failed ...");

    // imgui backend info
    const struct cimgui_info *info = cimgui_get_info();

    klog_info(" - platform : %s", info->platform_name);
    klog_info(" - renderer : %s", info->renderer_name);

    ////////////////////////////////////////

    ui_ptr()->is_initialized = true;
    ui_ptr()->show_imgui_demo_window = false;

    ////////////////////////////////////////

    // apply config
    ui_set_theme(ui_conf_ptr()->theme);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void ui_quit()
{
    csr_assert(ui_ptr()->is_initialized);

    klog_notice("quitting ui ...");

    cimgui_quit();
}

void ui_tick()
{
    csr_assert(ui_ptr()->is_initialized);

    bool update_content_scale = false; // FIXME

    cimgui_frame_begin(update_content_scale);

    ////////////////////////////////////////

    // draw workspace
    ui_workspace_tick(ui_ptr()->workspace);

    // draw imgui demo window
    if (ui_ptr()->show_imgui_demo_window) {
        igShowDemoWindow(&ui_ptr()->show_imgui_demo_window);
    }

    ////////////////////////////////////////

    cimgui_frame_end();
}

struct ui_conf* ui_get_config()
{
    csr_assert(ui_ptr()->is_initialized);

    return ui_conf_ptr();
}

void ui_toggle_imgui_demo_window()
{
    ui_ptr()->show_imgui_demo_window ^= 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// content scale
////////////////////////////////////////////////////////////////////////////////////////////////////
f32 ui_get_content_scale()
{
    csr_assert(ui_ptr()->is_initialized);

    f32 scale = kio_video_get_content_scale();

    if (ui_conf_ptr()->content_scale.use_custom_scale) {
        scale = ui_conf_ptr()->content_scale.scale_factor;
    }

    return scale;
}

void ui_set_content_scale(f32 scale)
{
    // FIXME check_range(min, max, value));
    check_expr(scale >= UI_CONTENT_SCALE_MIN && scale <= UI_CONTENT_SCALE_MAX);

    if (ui_conf_ptr()->content_scale.use_custom_scale) {
        ui_conf_ptr()->content_scale.scale_factor = scale;
    }

    // request scale update
    {
        // - load font at new size
        // - rebuild font atlas
        // - call style.ScaleAlleSizes() (on a ref. ImGuiStyle)

        klog_warn("ui_set_content_scale() not impl. yet");
    }

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// workspaces
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ui_workspace* ui_get_workspace()
{
    return ui_ptr()->workspace;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// menus
////////////////////////////////////////////////////////////////////////////////////////////////////
void ui_menu_init(struct ui_menu *menu, const char *title)
{
    check_ptr(menu);
    check_ptr(title);

    menu->title = strdup(title);

    menu->key = NULL;

    menu->draw_cb = NULL;
    menu->is_enabled_cb = NULL;

error:
    return;
}

void ui_menu_draw(struct ui_menu* menu, struct ui_style *style)
{
    check_ptr(menu);
    check_ptr(style);

    if (menu->is_enabled_cb && !menu->is_enabled_cb(menu)) return;

    if (igBeginMenu(menu->title, true))
    {
        if (menu->draw_cb) {
            menu->draw_cb(menu, style);
        }

        igEndMenu();
    }

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// windows, views
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _default_view_draw_cb(struct ui_view* view, struct ui_style *style)
{
    igText("No View available");
}

static void _default_window_push_properties_cb(struct ui_window* win)
{
    // FIXME need a better approach for generic styling options
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, make_ImVec2(8, 8));
}

static void _default_window_pop_properties_cb(struct ui_window* win)
{
    igPopStyleVar(1);
}

static void _init_view_defaults(struct ui_window *window)
{
    check_ptr(window);

    window->view.name = strdup(window->title);
    window->view.parent = window;
    window->view.draw_cb = _default_view_draw_cb;
    window->view.user_data = NULL;

error:
    return;
}

void ui_window_init(struct ui_window *window, const char *title, ui_window_init_view_cb_t init_view_cb)
{
    check_ptr(window);
    check_ptr(title);

    window->title = strdup(title);

    window->key = NULL;

    window->is_opened = false;
    window->is_viewport = false;

    window->flags = ImGuiWindowFlags_None;

    window->push_properties_cb = _default_window_push_properties_cb;
    window->pop_properties_cb = _default_window_pop_properties_cb;

    window->user_data = NULL;

    _init_view_defaults(window);

    if (init_view_cb) {
        init_view_cb(&window->view);
    }

error:
    return;
}

void ui_window_draw(struct ui_window* window, struct ui_style *style)
{
    check_ptr(window);
    check_ptr(style);

    if (!window->is_opened) return;

    ////////////////////////////////////////

    // push window style properties
    if (window->push_properties_cb) {
        window->push_properties_cb(window);
    }

    ////////////////////////////////////////

    // draw window
    if (igBegin(window->title, &window->is_opened, window->flags))
    {
        if (igIsWindowFocused(ImGuiFocusedFlags_ChildWindows) || igIsWindowAppearing())
        {
            // ...
        }

        if (window->view.draw_cb) {
            window->view.draw_cb(&window->view, style);
        }

        igEnd();
    }

    ////////////////////////////////////////

    // pop window style properties
    if (window->pop_properties_cb) {
        window->pop_properties_cb(window);
    }

error:
    return;
}
