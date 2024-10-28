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
    check_ptr(init_info->conf);

    check_expr(string_is_valid(init_info->imgui_ini_file));
    check_expr(string_is_valid(init_info->fonts_dir));

    ui_ptr()->conf = init_info->conf;
    ui_ptr()->workspace = init_info->workspace;
    ui_ptr()->fonts_dir = init_info->fonts_dir;

    ////////////////////////////////////////

    // init imgui
    check_expr(cimgui_init(init_info->imgui_ini_file) == RC_SUCCESS);

    // imgui backend info
    const struct cimgui_info *info = cimgui_get_info();

    klog_info(" - platform : %S", &info->platform_name);
    klog_info(" - renderer : %S", &info->renderer_name);

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
void ui_menu_init(struct ui_menu *menu, string_cstr title)
{
    check_ptr(menu);
    check_ptr(title);

    menu->key = NULL;
    menu->title = title;

    menu->draw_cb = NULL;
    menu->draw_cond_cb = NULL;

error:
    return;
}

void ui_menu_draw(struct ui_menu* menu, struct ui_style *style)
{
    check_ptr(menu);
    check_ptr(style);

    if (menu->draw_cond_cb && !menu->draw_cond_cb(menu)) return;

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

extern void init_screen_view(struct ui_view *view, void *screen);

void ui_view_init(struct ui_view *view, enum ui_view_type view_type, void* user_data)
{
    check_ptr(view);
    check_expr(view_type != UI_VIEW_TYPE_UNKNOWN);

    switch (view_type)
    {
        case UI_VIEW_TYPE_SCREEN:
            init_screen_view(view, user_data);
        break;

        case UI_VIEW_TYPE_CUSTOM: {
            view->type = view_type;
            view->draw_cb = _default_view_draw_cb;
            view->user_data = user_data;
        }
        break;
    }

error:
    return;
}

static void _default_window_push_properties_cb(struct ui_window* win) {}
static void _default_window_pop_properties_cb(struct ui_window* win) {}

void ui_window_init(struct ui_window *window, string_cstr title)
{
    check_ptr(window);
    check_ptr(title);

    window->key = NULL;
    window->title = title;
    window->is_opened = false;

    window->priv.flags = ImGuiWindowFlags_None;
    window->priv.push_properties_cb = _default_window_push_properties_cb;
    window->priv.pop_properties_cb = _default_window_pop_properties_cb;

    window->view.draw_cb = _default_view_draw_cb;
    window->view.parent = window;

error:
    return;
}

void ui_window_draw(struct ui_window* window, struct ui_style *style)
{
    check_ptr(window);
    check_ptr(style);

    if (!window->is_opened) return;

    ////////////////////////////////////////

    // initial window setup (no ini file present)
    {
        // window size
        struct vec2 win_size = window->priv.size;

        if (!vec2_equal(win_size, make_vec2_zero())) {
            igSetNextWindowSize(make_ImVec2_from_vec2(win_size), ImGuiCond_FirstUseEver);
        }

        // window position
        struct vec2 win_position = window->priv.position;

        if (!vec2_equal(win_position, make_vec2_zero())) {
            igSetNextWindowPos(make_ImVec2_from_vec2(win_position), ImGuiCond_FirstUseEver, make_ImVec2_zero());
        }
    }

    // push window style properties
    if (window->priv.push_properties_cb) {
        window->priv.push_properties_cb(window);
    }

    ////////////////////////////////////////

    // draw window
    if (igBegin(window->title, &window->is_opened, window->priv.flags))
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
    if (window->priv.pop_properties_cb) {
        window->priv.pop_properties_cb(window);
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
        my_font->info.name = ui_conf_ptr()->font.name;
        my_font->info.size = ui_conf_ptr()->font.size;
        my_font->info.size_scaled = ceilf(my_font->info.size * scale_factor);

        string_cstr font_path = (string_cstr)string_create_fmt(kio_mem_get_main_arena(), "%S/%s",
            &ui_ptr()->fonts_dir, my_font->info.name).ptr;

        my_font->data = ImFontAtlas_AddFontFromFileTTF(font_atlas, font_path, my_font->info.size_scaled, NULL, NULL);

        if (my_font->data)
        {
            igGetIO()->FontDefault = my_font->data;
        }
        else
        {
            klog_error("could not load font : %s ...", font_path);

            ui_conf_ptr()->font.use_custom_font = false;
        }
    }

    ////////////////////////////////////////

    ImFontAtlas_Build(font_atlas);
}
