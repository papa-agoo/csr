////////////////////////////////////////////////////////////////////////////////////////////////////

#include "application.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct application g_application = {0};

struct application* application_ptr()
{
    return &g_application;
}

static struct hid_callbacks* _get_hid_callbacks()
{
    // input layer : application
    if (application_has_input_focus()) {
        return &application_ptr()->callbacks.hid;
    }

    // input layer : applet
    struct applet *applet = applet_mgr_get_applet();

    if (applet) {
        return applet_get_hid_callbacks(applet);
    }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// env
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e init_env_vars()
{
    klog_info("setting env vars ...");

    check_expr(kio_env_set_var("USER_HOME_DIR", platform_get_user_home_dir()));

    check_expr(kio_env_set_var("APP_HOME_DIR", ENV_APP_HOME_DIR));
    check_expr(kio_env_set_var("APP_CONFIG_DIR", ENV_APP_CONFIG_DIR));
    check_expr(kio_env_set_var("APPLET_CONFIG_DIR", ENV_APPLET_CONFIG_DIR));

    check_expr(kio_env_set_var("APPLET_DIR", ENV_APPLET_DIR));
    check_expr(kio_env_set_var("RESOURCE_DIR", ENV_RESOURCE_DIR));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e init_app_home_dir()
{
    struct string app_home_dir = kio_env_expand_str(ENV_APP_HOME_DIR);

    if (!fio_fs_is_directory(app_home_dir))
    {
        klog_info("initializing application home dir (%s)", ENV_APP_HOME_DIR);

        fio_fs_create_directory(kio_env_expand_str(ENV_APP_CONFIG_DIR));
    }

    klog_trace("using app home dir : %S", &app_home_dir);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e init_user_config()
{
    struct application_conf *conf = application_conf_ptr();

    struct string ini_file = kio_env_expand_str(ENV_APP_INI_FILE);

    klog_trace("loading user config ( %S ) ...", &ini_file);

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
    application_ptr()->has_input_focus = kio_hid_kbd_key_down(KBD_KEY_LEFT_SHIFT);

    applet_mgr_tick();

    ////////////////////////////////////////

    const struct ui_theme_info *theme_info = ui_get_theme_info();
    check_ptr(theme_info);

    struct xgl_render_pass_info pass_info = {0};
    pass_info.name = make_string("Application");
    pass_info.clear_values.color = theme_info->style.color.background;
    pass_info.framebuffer = kio_video_get_framebuffer();

    if (xgl_begin_render_pass(&pass_info))
    {
        // struct screen *screen = foo_get_active_screen();

        // if (screen && screen_is_fullscreen(screen))
        // {
        //     // applet loaded AND active screen is in fullscreen mode
        //     render_fullscreen_quad();
        //
        //     kio_video_draw_fullscreen_quad(make_rect(x, y, w, h), sampler, texture);
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
    application_stop();
}

static void on_keyboard_key_down(struct event* event)
{
    check_ptr(event);

    struct hid_callbacks *hid_cbs = _get_hid_callbacks();

    if (hid_cbs && hid_cbs->on_kbd_key_down) {
        hid_cbs->on_kbd_key_down(&event->keyboard);
    }

error:
    return;
}

static void on_keyboard_key_up(struct event* event)
{
    check_ptr(event);

    struct hid_callbacks *hid_cbs = _get_hid_callbacks();

    if (hid_cbs && hid_cbs->on_kbd_key_up) {
        hid_cbs->on_kbd_key_up(&event->keyboard);
    }

error:
    return;
}

static void on_mouse_button_down(struct event* event)
{
    check_ptr(event);

    struct hid_callbacks *hid_cbs = _get_hid_callbacks();

    if (hid_cbs && hid_cbs->on_mouse_button_down) {
        hid_cbs->on_mouse_button_down(&event->mouse);
    }

error:
    return;
}

static void on_mouse_button_up(struct event* event)
{
    check_ptr(event);

    struct hid_callbacks *hid_cbs = _get_hid_callbacks();

    if (hid_cbs && hid_cbs->on_mouse_button_up) {
        hid_cbs->on_mouse_button_up(&event->mouse);
    }

error:
    return;
}

static void on_mouse_wheel_spin(struct event* event)
{
    check_ptr(event);

    struct hid_callbacks *hid_cbs = _get_hid_callbacks();

    if (hid_cbs && hid_cbs->on_mouse_wheel_spin) {
        hid_cbs->on_mouse_wheel_spin(&event->mouse);
    }

error:
    return;
}

static void on_mouse_move(struct event* event)
{
    check_ptr(event);

    struct hid_callbacks *hid_cbs = _get_hid_callbacks();

    if (hid_cbs && hid_cbs->on_mouse_move) {
        hid_cbs->on_mouse_move(&event->mouse);
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
    info.name = make_string("Main");
    info.enable_docking = true;
    info.workspace_tick_cb = on_workspace_main_tick;

    struct ui_workspace *workspace = ui_workspace_create(&info);
    check_ptr(workspace);

    // add application ctx to workspace
    struct ui_ctx *application_ctx = ui_ctx_create(make_string("Application"));
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
// applets
////////////////////////////////////////////////////////////////////////////////////////////////////
static void on_post_applet_load(struct applet *applet)
{
    check_ptr(applet);

    klog_trace("attaching applet ui to frontend (%s) ...", applet_get_filename(applet));
    {
        struct ui_ctx *ctx = applet_get_ui_ctx(applet);

        ui_workspace_add_context(ui_get_workspace(), ctx);

        init_frontend_applet_ctx(ctx);
    }

error:
    return;
}

static void on_pre_applet_unload(struct applet *applet)
{
    check_ptr(applet);

    klog_trace("detaching applet ui from frontend (%s) ...", applet_get_filename(applet));
    {
        struct ui_ctx *ctx = applet_get_ui_ctx(applet);

        ui_workspace_remove_context(ui_get_workspace(), ctx);
    }

error:
    return;
}

static void init_applet_mgr_config()
{
    struct application_conf *conf = application_conf_ptr();

    struct applet_mgr_conf *mgr_conf = &conf->applet_mgr;
    {
        applet_mgr_conf_defaults(mgr_conf);

        config_map_bool(conf->user, "applet_mgr:remember_applet", &mgr_conf->remember_applet);
        config_map_str(conf->user, "applet_mgr:applet_name", &mgr_conf->applet_name);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// application
////////////////////////////////////////////////////////////////////////////////////////////////////
static void on_application_kbd_key_down(struct keyboard_event* event)
{
    check_ptr(event);

    switch (event->key)
    {
        // quit application
        case KBD_KEY_ESCAPE: {
            application_stop();
        }
        break;

        case KBD_KEY_Q: {
            applet_mgr_request_applet_unload();
        }
        break;

        // viewport: toggle fullscreen
        case KBD_KEY_F: {
            klog_warn("screen_toggle_fullscreen() not implemented yet");
        }
        break;

        // viewport: reset scale factor
        case KBD_KEY_1: {
            klog_warn("screen_reset_scale() not implemented yet");
        }
        break;

        // toggle ui
        case KBD_KEY_F1: {
            klog_warn("ui_toggle() not implemented yet");
        }
        break;

        // toggle vsync
        case KBD_KEY_F10: {
            kio_video_toggle_vsync();
        }
        break;

        // toggle fullscreen mode
        case KBD_KEY_F11: {
            kio_video_toggle_fullscreen();
        }
        break;

        // toggle ui main menu
        case KBD_KEY_LEFT_ALT: {
            application_ptr()->show_main_menu ^= 1;
        }
        break;

        default: {
            klog_error("key not handled : %s", keyboard_key_cstr(event->key));
        }
    }

error:
    return;
}

static void on_application_mouse_wheel_spin(struct mouse_event *event)
{
    check_ptr(event);

    // viewport: scale up
    if (event->wy > 0)
    {
        klog_warn("screen_scale_up() not implemented yet");

        // FIXME (screen_size == os_window_size) => go fullscreen
    }

    // viewport: scale down
    if (event->wy < 0)
    {
        klog_warn("screen_scale_down() not implemented yet");

        // FIXME (screen_size == os_window_size) => go windowed mode
    }

error:
    return;
}

result_e application_init()
{
    csr_assert(!application_ptr()->is_initialized);

    clog_info("************************************************************");
    clog_info("   ____  ____  ____  ____                                   ");
    clog_info("  /\\   \\/\\   \\/\\   \\/\\   \\                          ");
    clog_info(" /  \\___\\ \\___\\ \\___\\ \\___\\                         ");
    clog_info(" \\  / __/_/   / /   / /   /                                ");
    clog_info("  \\/_/\\   \\__/\\/___/\\/___/                             ");
    clog_info("    /  \\___\\    /  \\___\\                                ");
    clog_info("    \\  / __/_  _\\  /   /                                  ");
    clog_info("     \\/_/\\   \\/\\ \\/___/                                ");
    clog_info("       /  \\__/  \\___\\                                    ");
    clog_info("       \\  / _\\  /   /            ___________ ____         ");
    clog_info("        \\/_/\\ \\/___/            / ____/ ___// __ \\      ");
    clog_info("          /  \\___\\             / /    \\__ \\/ /_/ /      ");
    clog_info("          \\  /   /            / /___ ___/ / _, _/          ");
    clog_info("           \\/___/             \\____//____/_/ |_|  v%s     ", ENV_APP_VERSION);
    clog_info("                                                            ");
    clog_info("************************************************************");

    struct application_conf *conf = application_conf_ptr();

    ////////////////////////////////////////

    klog_notice("initializing env ...");
    {
        check_result(init_env_vars());
        check_result(init_app_home_dir());
        check_result(init_user_config());
    }

    ////////////////////////////////////////

    klog_notice("initializing kernel ...");
    {
        init_kernel_config();

        struct kernel_init_info init_info = {0};
        init_info.name = make_string(ENV_APP_NAME);
        init_info.kernel_tick_cb = on_kernel_tick;

        init_info.conf.core = &conf->kernel.core;
        init_info.conf.video = &conf->kernel.video;
        init_info.conf.audio = &conf->kernel.audio;

        check_result(kernel_init(&init_info));

        // global events
        event_bus_register_handler(EVENT_WINDOW_CLOSE, on_window_close);

        event_bus_register_handler(EVENT_KBD_KEY_DOWN, on_keyboard_key_down);
        event_bus_register_handler(EVENT_KBD_KEY_UP, on_keyboard_key_up);

        event_bus_register_handler(EVENT_MOUSE_BUTTON_DOWN, on_mouse_button_down);
        event_bus_register_handler(EVENT_MOUSE_BUTTON_UP, on_mouse_button_up);
        event_bus_register_handler(EVENT_MOUSE_MOVE, on_mouse_move);
        event_bus_register_handler(EVENT_MOUSE_WHEEL, on_mouse_wheel_spin);
    }

    ////////////////////////////////////////

    klog_notice("initializing frontend ...");
    {
        application_ptr()->show_main_menu = true;

        init_frontend_config();

        struct ui_init_info init_info = {0};
        init_info.imgui_ini_file = kio_env_expand_str(ENV_IMGUI_INI_FILE);
        init_info.fonts_dir = kio_env_expand_str("{RESOURCE_DIR}/fonts");
        init_info.conf = &conf->ui;
        init_info.workspace = create_frontend_workspace();

        check_result(ui_init(&init_info));
    }

    ////////////////////////////////////////

    klog_notice("initializing applet manager ...");
    {
        init_applet_mgr_config();

        struct applet_mgr_init_info init_info = {0};
        init_info.conf = &conf->applet_mgr;
        init_info.db_scan_path = make_string(ENV_APPLET_DIR);
        init_info.callbacks.on_post_applet_load = on_post_applet_load;
        init_info.callbacks.on_pre_applet_unload = on_pre_applet_unload;

        check_result(applet_mgr_init(&init_info));
    }

    ////////////////////////////////////////

    // application hid callbacks
    {
        struct hid_callbacks *hid = &application_callbacks_ptr()->hid;
        hid->on_kbd_key_down = on_application_kbd_key_down;
        hid->on_mouse_wheel_spin = on_application_mouse_wheel_spin;
    }

    // fullscreen quad resources (will be moved to rsx)
    {
        // https://learnopengl.com/Advanced-OpenGL/Framebuffers
    }

    application_ptr()->is_initialized = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void application_quit()
{
    csr_assert(application_ptr()->is_initialized);

    if (application_ptr()->is_running)
    {
        clog_warn("application still running!? ... stopping now!");

        application_stop();
    }

    ////////////////////////////////////////

    klog_notice("quitting applet manager ...");
    {
        applet_mgr_quit();
    }

    klog_notice("quitting frontend ...");
    {
        ui_quit();
    }

    klog_notice("quitting env ...");
    {
        struct application_conf *conf = application_conf_ptr();

        config_flush(conf->user);
        config_destroy(conf->user);
    }

    klog_notice("quitting kernel ...");
    {
        kernel_quit();
    }
}

void application_run()
{
    csr_assert(application_ptr()->is_initialized);

    application_ptr()->is_running = true;

    kernel_run();
}

void application_stop()
{
    csr_assert(application_ptr()->is_running);

    kernel_stop();

    application_ptr()->is_running = false;
}

void application_request_restart()
{
    csr_assert(application_ptr()->is_running);

    // FIXME send custom "application restart needed" event

    if (!application_ptr()->needs_restart) {
        klog_warn("application restart required");
    }

    application_ptr()->needs_restart = true;
}

bool application_has_input_focus()
{
    return application_ptr()->has_input_focus;
}
