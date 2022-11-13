////////////////////////////////////////////////////////////////////////////////////////////////////

#include "application.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct application g_application;

struct application* application_ptr()
{
    return &g_application;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// env
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e init_env_vars()
{
    klog_info("setting env vars ...");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e init_app_home_dir()
{
    const char *app_home_dir = "${HOME}/agummer/.csr";

    klog_info("using app home dir : %s", app_home_dir);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e init_user_config()
{
    struct application_conf *conf = application_conf_ptr();

    const char *ini_file = ENV_APP_INI_FILE;

    klog_info("loading user config ( %s ) ...", ini_file);

    conf->user = config_create_from_ini(ini_file);
    check_ptr(conf->user);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// kernel
////////////////////////////////////////////////////////////////////////////////////////////////////
static void on_kernel_tick()
{
    // applet_mgr_tick();

    ////////////////////////////////////////

    const struct ui_theme_info *theme_info = ui_get_theme_info(ui_get_theme());
    check_ptr(theme_info);

    struct xgl_render_pass_info pass_info = {0};
    pass_info.name = "Application";
    pass_info.clear_values.color = theme_info->style.color.background;
    pass_info.framebuffer = kio_video_get_framebuffer();

    if (xgl_begin_render_pass(&pass_info))
    {
        // struct screen *screen = foo_get_active_screen();

        // if (screen && screen_is_fullscreen(screen))
        // {
        //     // applet loaded AND active screen is in fullscreen mode
        //     render_fullscreen_quad();
        // }
        // else
        // {
        //     // no applet loaded OR windowed mode screen(s)
        //     ui_tick();
        // }

        ui_tick();

        xgl_end_render_pass();
    }

error:
    return;
}

static void on_window_close(struct event* event)
{
    kernel_stop();
}

static void on_keyboard_key_down(struct event* event)
{
    check_ptr(event);

    if (event->keyboard.key == KBD_KEY_ESCAPE) {
        kernel_stop();
    }

error:
    return;
}

static void init_kernel_config()
{
    struct application_conf *conf = application_conf_ptr();

    ////////////////////////////////////////

    // core
    struct ksrv_core_conf *core_conf = &conf->kernel.core;
    {
        ksrv_core_conf_defaults(core_conf);

        // log
        config_map_int(conf->user, "kernel.core/log:max_messages", &core_conf->log_max_messages);
        config_map_bool(conf->user, "kernel.core/log:show_trace_messages", &core_conf->log_show_trace_messages);
    }

    ////////////////////////////////////////

    // video
    struct ksrv_video_conf *video_conf = &conf->kernel.video;
    {
        ksrv_video_conf_defaults(video_conf);

        // window
        config_map_vec2(conf->user, "kernel.video/window:resolution", &video_conf->window.video_mode.resolution);
        config_map_bool(conf->user, "kernel.video/window:fullscreen", &video_conf->window.fullscreen);
        config_map_bool(conf->user, "kernel.video/window:vsync", &video_conf->window.vsync);

        // xgl
        config_map_int(conf->user, "kernel.video/xgl:msaa_count", &video_conf->xgl.msaa_count);
        config_map_bool(conf->user, "kernel.video/xgl:debug_mode", &video_conf->xgl.debug_mode);

        // create a valid video mode
        video_conf->window.video_mode = video_mode_create(video_conf->window.video_mode.resolution);
    }

    ////////////////////////////////////////

    // audio
    struct ksrv_audio_conf *audio_conf = &conf->kernel.audio;
    {
        ksrv_audio_conf_defaults(audio_conf);

        config_map_int(conf->user, "kernel.audio/xal:volume", &audio_conf->volume);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// frontend
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e init_frontend_applet_ctx(struct ui_ctx *ctx);
result_e init_frontend_application_ctx(struct ui_ctx *ctx);

static void init_frontend_config()
{
    struct application_conf *conf = application_conf_ptr();

    ////////////////////////////////////////

    struct ui_conf *ui_conf = &conf->ui;
    {
        ui_conf_defaults(ui_conf);

        // content scale
        config_map_bool(conf->user, "frontend.ui/content_scale:use_custom_scale", &ui_conf->content_scale.use_custom_scale);
        config_map_float(conf->user, "frontend.ui/content_scale:scale_factor", &ui_conf->content_scale.scale_factor);

        // custom font
        config_map_bool(conf->user, "frontend.ui/font:use_custom_font", &ui_conf->font.use_custom_font);
        config_map_str(conf->user, "frontend.ui/font:name", &ui_conf->font.name);
        config_map_float(conf->user, "frontend.ui/font:size", &ui_conf->font.size);

        // theme
        config_map_int(conf->user, "frontend.ui/theme:type", (s32*) &ui_conf->theme);
    }
}

static struct ui_workspace* create_frontend_workspace()
{
    void on_workspace_main_tick(struct ui_workspace * workspace);

    // create workspace
    struct ui_workspace_create_info info = {0};
    info.name = "Main";
    info.enable_docking = true;
    info.workspace_tick_cb = on_workspace_main_tick;

    struct ui_workspace *workspace = ui_workspace_create(&info);
    check_ptr(workspace);

    // add application ctx to workspace
    struct ui_ctx *application_ctx = ui_ctx_create("Application");
    check_ptr(application_ctx);

    init_frontend_application_ctx(application_ctx);

    ui_workspace_add_context(workspace, application_ctx);

    return workspace;

error:
    if (workspace) {
        ui_workspace_destroy(workspace);
    }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e application_init()
{
    clog_info("************************************************************");
    clog_info("     ___________ ____                                       ");
    clog_info("    / ____/ ___// __ \\                                     ");
    clog_info("   / /    \\__ \\/ /_/ /                                    ");
    clog_info("  / /___ ___/ / _, _/                                       ");
    clog_info("  \\____//____/_/ |_| v%s                                   ", ENV_APP_VERSION);
    clog_info("                                                            ");
    clog_info("************************************************************");

    struct application_conf *conf = application_conf_ptr();

    ////////////////////////////////////////

    klog_info("initializing env ...");
    {
        check_result(init_env_vars(), "could not init env vars");
        check_result(init_app_home_dir(), "could not init app home dir");
        check_result(init_user_config(), "could not init user config");
    }

    ////////////////////////////////////////

    klog_info("initializing kernel ...");
    {
        init_kernel_config();

        struct kernel_init_info init_info = {0};
        init_info.name = ENV_APP_NAME;
        init_info.kernel_tick_cb = on_kernel_tick;

        init_info.conf.core = &conf->kernel.core;
        init_info.conf.video = &conf->kernel.video;
        init_info.conf.audio = &conf->kernel.audio;

        check_result(kernel_init(&init_info), "could not init kernel");

        // global events
        event_bus_register_handler(EVENT_WINDOW_CLOSE, on_window_close);
        event_bus_register_handler(EVENT_KBD_KEY_DOWN, on_keyboard_key_down);
    }

    ////////////////////////////////////////

    klog_info("initializing frontend ...");
    {
        init_frontend_config();

        struct ui_init_info init_info = {0};
        init_info.imgui_ini_file = ENV_IMGUI_INI_FILE;
        init_info.conf = &conf->ui;
        init_info.workspace = create_frontend_workspace();

        check_result(ui_init(&init_info), "could not init frontend");
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void application_quit()
{
    struct application_conf *conf = application_conf_ptr();

    ////////////////////////////////////////

    klog_info("quitting frontend ...");
    {
        ui_quit();
    }

    klog_info("quitting kernel ...");
    {
        kernel_quit();
    }

    klog_info("quitting env ...");
    {
        config_flush(conf->user);
        config_destroy(conf->user);
    }
}

result_e main()
{
    result_e result = application_init();
    check_result(result, "could not init application");

    kernel_run();

    result = RC_SUCCESS;

error:
    application_quit();

    return result;
}
