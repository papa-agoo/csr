////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/ui_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct ui g_ui;

struct ui* ui_ptr()
{
    return &g_ui;
}

static void _ui_tick_dummy() {}

result_e ui_init(struct ui_init_info *init_info)
{
    csr_assert(!ui_ptr()->is_initialized);

    check_ptr(init_info);
    check_ptr(init_info->imgui_ini_file);
    check_ptr(init_info->conf);

    ui_ptr()->conf = init_info->conf;

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

    // set tick callback
    ui_ptr()->ui_tick_cb = init_info->ui_tick_cb;

    if (!ui_ptr()->ui_tick_cb) {
        ui_ptr()->ui_tick_cb = _ui_tick_dummy;
    }

    // apply config
    ui_set_theme(ui_conf_ptr()->theme);

    ////////////////////////////////////////

    ui_ptr()->is_initialized = true;

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

    bool update_content_scale = false;

    cimgui_frame_begin(update_content_scale);

    ui_ptr()->ui_tick_cb();

    cimgui_frame_end();
}
