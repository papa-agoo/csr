////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// core service
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _draw_core_log_max_messages()
{
    struct ksrv_core_conf *conf = &application_conf_ptr()->kernel.core;

    if (igDragInt("##log_max_messages", &conf->log_max_messages, 1, 128, 1024, "%d", 0))
    {
        klog_warn("event : application_request_restart()");
    }
}

static void _draw_core_log_show_trace_messages()
{
    struct ksrv_core_conf *conf = &application_conf_ptr()->kernel.core;

    igCheckbox("##log_show_trace_messages", &conf->log_show_trace_messages);
}

static void _draw_core_srv_settings()
{
    // logger set
    static struct ui_property properties_logger[] = {
        {
            .name = "Max Log Messages",
            .draw_cb = _draw_core_log_max_messages,
        },
        {
            .name = "Show Trace Messages",
            .draw_cb = _draw_core_log_show_trace_messages,
        },
    };

    struct ui_property_set set_logger = {0};
    set_logger.name = "Logger";
    set_logger.properties = properties_logger;
    set_logger.property_count = COUNT_OF(properties_logger);

    ////////////////////////////////////////

    // draw sets
    struct ui_property_set* sets[] = {
        &set_logger,
    };

    struct ui_property_set_group group = {0};
    group.name = "Core";
    group.sets = sets;
    group.set_count = COUNT_OF(sets);

    ui_property_set_group_draw(&group);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// video service
////////////////////////////////////////////////////////////////////////////////////////////////////
static bool _cond_custom_resolution_selected()
{
    struct ksrv_video_conf *conf = &application_conf_ptr()->kernel.video;

    enum video_mode_preset vm_preset = conf->window.video_mode.preset;

    return vm_preset == VIDEO_MODE_PRESET_CUSTOM && !conf->window.fullscreen;
}

static void _draw_video_resolution()
{
    u32 preset_count = 0;
    const char **preset_list = NULL;

    video_mode_preset_get_list(&preset_count, &preset_list);

    ////////////////////////////////////////

    struct ksrv_video_conf *conf = &application_conf_ptr()->kernel.video;
    struct video_mode *vm = &conf->window.video_mode;

    if (conf->window.fullscreen)
    {
        igText("%.0fx%.0f", vm->resolution.w, vm->resolution.h);

        return;
    }

    ////////////////////////////////////////


    if (igCombo_Str_arr("##resolution", (u32*) &vm->preset, preset_list, preset_count, preset_count))
    {
        video_mode_preset_get_resolution(vm->preset, &vm->resolution);

        kio_video_set_resolution(vm->resolution);
    }
}

static void _draw_video_custom_resolution()
{
    struct video_mode *vm = &application_conf_ptr()->kernel.video.window.video_mode;

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

    if (igInputFloat2("##custom_resolution", (f32*) &vm->resolution.data, "%.0f", flags))
    {
        kio_video_set_resolution(vm->resolution);
    }
}

static void _draw_video_window_vsync()
{
    bool *vsync = &application_conf_ptr()->kernel.video.window.vsync;

    if (igCheckbox("##window_vsync", vsync))
    {
        kio_video_enable_vsync(*vsync);
    }
}

static void _draw_video_window_fullscreen()
{
    bool *fullscreen = &application_conf_ptr()->kernel.video.window.fullscreen;

    if (igCheckbox("##window_fullscreen", fullscreen))
    {
        kio_video_enable_fullscreen(*fullscreen);
    }
}

static void _draw_video_xgl_msaa_count()
{
    struct ksrv_video_conf *conf = &application_conf_ptr()->kernel.video;

    // FIXME 0 => sample_count_max
    if (igSliderInt("##xgl_msaa_count", &conf->xgl.msaa_count, 0, 32, "%d", 0))
    {
        // FIXME valid values : 0, 2, 4, 8
        klog_warn("event : application_request_restart()");
    }
}

static void _draw_video_xgl_debug_mode()
{
    struct ksrv_video_conf *conf = &application_conf_ptr()->kernel.video;

    if (igCheckbox("##xgl_debug_mode", &conf->xgl.debug_mode))
    {
        klog_warn("event : application_request_restart()");
    }
}

static void _draw_video_srv_settings()
{
    // window set
    struct ui_property properties_window[] = {
        {
            .name = "Resolution",
            .draw_cb = _draw_video_resolution
        },
        {
            .draw_cb = _draw_video_custom_resolution,
            .draw_cond_cb = _cond_custom_resolution_selected,
        },
        {
            .name = "Fullscreen",
            .draw_cb = _draw_video_window_fullscreen
        },
        {
            .name = "V-Sync",
            .draw_cb = _draw_video_window_vsync
        },
    };

    struct ui_property_set set_window = {0};
    set_window.name = "Window";
    set_window.properties = properties_window;
    set_window.property_count = COUNT_OF(properties_window);

    // XGL set
    static struct ui_property properties[] = {
        // {
        //     .name = "MSAA Count",
        //     .tooltip = "0 = Disabled",
        //     .draw_cb = _draw_video_xgl_msaa_count
        // },
        {
            .name = "Debug Context",
            .draw_cb = _draw_video_xgl_debug_mode
        },
    };

    struct ui_property_set set_xgl = {0};
    set_xgl.name = "XGL";
    set_xgl.properties = properties;
    set_xgl.property_count = COUNT_OF(properties);

    ////////////////////////////////////////

    // draw sets
    struct ui_property_set* sets[] = {
        &set_window,
        &set_xgl
    };

    struct ui_property_set_group group = {0};
    group.name = "Video";
    group.sets = sets;
    group.set_count = COUNT_OF(sets);

    ui_property_set_group_draw(&group);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// audio service
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _draw_audio_xal_volume()
{
    struct ksrv_audio_conf *conf = &application_conf_ptr()->kernel.audio;

    igDragInt("##xal_volume", &conf->volume, 1, 0, 100, "%3d%%", 0);
}

static void _draw_audio_srv_settings()
{
    // XAL set
    struct ui_property properties_xal[] = {
        {
            .name = "Volume",
            .tooltip = "not impl. yet :)",
            .draw_cb = _draw_audio_xal_volume
        },
    };

    struct ui_property_set set_xal = {0};
    set_xal.name = "XAL";
    set_xal.properties = properties_xal;
    set_xal.property_count = COUNT_OF(properties_xal);

    ////////////////////////////////////////

    // draw sets
    struct ui_property_set* sets[] = {
        &set_xal,
    };

    struct ui_property_set_group group = {0};
    group.name = "Audio";
    group.sets = sets;
    group.set_count = COUNT_OF(sets);

    ui_property_set_group_draw(&group);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

void _draw_kernel_settings_page(struct ui_view* view)
{
    _draw_core_srv_settings();
    _draw_video_srv_settings();
    _draw_audio_srv_settings();
}
