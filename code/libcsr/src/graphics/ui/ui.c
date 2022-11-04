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

    ui_ptr()->is_initialized = true;

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

    bool update_content_scale = false;

    cimgui_frame_begin(update_content_scale);

    ui_ptr()->ui_tick_cb();

    cimgui_frame_end();
}

struct ui_conf* ui_get_config()
{
    csr_assert(ui_ptr()->is_initialized);

    return ui_conf_ptr();
}

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
