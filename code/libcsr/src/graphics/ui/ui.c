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
    check_ptr(init_info->fonts_dir);
    check_ptr(init_info->conf);

    ui_ptr()->conf = init_info->conf;
    ui_ptr()->workspace = init_info->workspace;
    ui_ptr()->fonts_dir = strdup(init_info->fonts_dir);

    ////////////////////////////////////////

    // init imgui
    result_e result = cimgui_init(init_info->imgui_ini_file);
    check(R_SUCCESS(result), "cimgui_init() failed ...");

    // imgui backend info
    const struct cimgui_info *info = cimgui_get_info();

    klog_info(" - platform : %s", info->platform_name);
    klog_info(" - renderer : %s", info->renderer_name);

    ////////////////////////////////////////

    ui_ptr()->is_initialized = true;
    ui_ptr()->show_imgui_demo_window = false;
    ui_ptr()->update_content_scale = false;

    ////////////////////////////////////////

    ui_build_fonts();

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

    ////////////////////////////////////////

    bool rebuild_textures = ui_ptr()->update_content_scale;

    if (ui_ptr()->update_content_scale)
    {
        ui_build_fonts();

        ui_set_theme(ui_conf_ptr()->theme);

        ui_ptr()->update_content_scale = false;
    }

    cimgui_frame_begin(rebuild_textures);

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
// fonts
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ui_font_info ui_get_font_info()
{
    // FIXME return ui_ptr()->font.current;

    if (ui_conf_ptr()->font.use_custom_font) {
        return ui_ptr()->font.custom.info;
    }

    return ui_ptr()->font.builtin.info;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// content scale
////////////////////////////////////////////////////////////////////////////////////////////////////
s32 ui_get_content_scale()
{
    return ui_get_content_scale_factor() * 100;
}

void ui_set_content_scale(s32 scale)
{
    ui_set_content_scale_factor(scale / 100.0);
}

f32 ui_get_content_scale_factor()
{
    csr_assert(ui_ptr()->is_initialized);

    f32 scale = kio_video_get_content_scale();

    if (ui_conf_ptr()->content_scale.use_custom_scale) {
        scale = ui_conf_ptr()->content_scale.scale_factor;
    }

    return scale;
}

void ui_set_content_scale_factor(f32 factor)
{
    // FIXME check_range(min, max, value));
    check_expr(factor >= UI_CONTENT_SCALE_FACTOR_MIN && factor <= UI_CONTENT_SCALE_FACTOR_MAX);

    if (ui_conf_ptr()->content_scale.use_custom_scale) {
        ui_conf_ptr()->content_scale.scale_factor = factor;
    }

    // request scale update
    ui_ptr()->update_content_scale = true;

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
        // FIXME collect window related events (focus, size, appear, ...)
        if (igIsWindowFocused(ImGuiFocusedFlags_ChildWindows) || igIsWindowAppearing())
        {
            ui_workspace_set_focused_window(ui_ptr()->workspace, window);
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

struct ui_window* ui_window_get_focused()
{
    check_ptr(ui_ptr()->workspace);

error:
    return NULL;
}

void ui_window_set_focused(struct ui_window* window)
{
    check_ptr(window);

    // track focused window internally
    ui_workspace_set_focused_window(ui_ptr()->workspace, window);

    // actually focus the window :)
    igSetWindowFocus_Str(window->title);

error:
    return;
}

bool ui_window_is_focused(struct ui_window* window)
{
    check_ptr(window);

    return ui_workspace_get_focused_window(ui_ptr()->workspace) == window;

error:
    return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// priv
////////////////////////////////////////////////////////////////////////////////////////////////////
void ui_build_fonts()
{
    ImFontAtlas *font_atlas = igGetIO()->Fonts;

    ImFontAtlas_Clear(font_atlas);

    ////////////////////////////////////////

    f32 scale_factor = ui_get_content_scale_factor();

    // builtin font
    {
        struct ui_font *my_font = &ui_ptr()->font.builtin;
        my_font->info.name = "ProggyClean.ttf";
        my_font->info.size = 13.0;
        my_font->info.size_scaled = ceilf(my_font->info.size * scale_factor);

        ImFontConfig *config = ImFontConfig_ImFontConfig();
        config->SizePixels = my_font->info.size_scaled;

        my_font->data = ImFontAtlas_AddFontDefault(font_atlas, config);

        igGetIO()->FontDefault = my_font->data;
    }

    // custom font
    if (ui_conf_ptr()->font.use_custom_font)
    {
        struct ui_font *my_font = &ui_ptr()->font.custom;
        my_font->info.name = strdup(ui_conf_ptr()->font.name);
        my_font->info.size = ui_conf_ptr()->font.size;
        my_font->info.size_scaled = ceilf(my_font->info.size * scale_factor);

        // FIXME make_string(...)
        char str_buf[256];
        snprintf(str_buf, 256, "%s/%s", ui_ptr()->fonts_dir, my_font->info.name);

        my_font->data = ImFontAtlas_AddFontFromFileTTF(font_atlas, str_buf, my_font->info.size_scaled, NULL, NULL);

        if (my_font->data)
        {
            igGetIO()->FontDefault = my_font->data;
        }
        else
        {
            klog_error("could not load font : %s ...", str_buf);

            ui_conf_ptr()->font.use_custom_font = false;
        }
    }

    ////////////////////////////////////////

    ImFontAtlas_Build(font_atlas);
}
