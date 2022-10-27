////////////////////////////////////////////////////////////////////////////////////////////////////

#define KLOG_MODULE_NAME application
#include <csr/kernel.h>

#include <csr/graphics/ui.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// (x) application stub + ui
// ( ) persistent user configs (${HOME}/.csr)
// ( ) ...

////////////////////////////////////////////////////////////////////////////////////////////////////

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

static void on_ui_tick()
{
    static bool show_demo_win = true;

    if (igBegin("Options", NULL, 0))
    {
        igCheckbox("Show ImGui Demo Window", &show_demo_win);

        if (show_demo_win) {
            igShowDemoWindow(&show_demo_win);
        }

        igEnd();
    }
}

static void on_kernel_tick()
{
    const struct ui_theme_info *theme_info = ui_get_theme_info(ui_get_theme());
    check_ptr(theme_info);

    struct xgl_render_pass_info pass_info = {0};
    pass_info.name = "Application";
    pass_info.clear_values.color = theme_info->style.color.background;
    pass_info.framebuffer = kio_video_get_framebuffer();

    if (xgl_begin_render_pass(&pass_info))
    {
        ui_tick();

        xgl_end_render_pass();
    }

error:
    return;
}

result_e main()
{
    clog_info("************************************************************");
    clog_info("     ___________ ____                                       ");
    clog_info("    / ____/ ___// __ \\                                     ");
    clog_info("   / /    \\__ \\/ /_/ /                                    ");
    clog_info("  / /___ ___/ / _, _/                                       ");
    clog_info("  \\____//____/_/ |_|                                       ");
    clog_info("                                                            ");
    clog_info("************************************************************");

    ////////////////////////////////////////

    // init kernel
    {
        struct ksrv_core_conf core_conf = {0};
        ksrv_core_conf_defaults(&core_conf);

        struct ksrv_video_conf video_conf = {0};
        ksrv_video_conf_defaults(&video_conf);

        struct ksrv_audio_conf audio_conf = {0};
        ksrv_audio_conf_defaults(&audio_conf);

        ////////////////////////////////////////

        struct kernel_init_info init_info = {0};
        init_info.name = "CSR";
        init_info.kernel_tick_cb = on_kernel_tick;

        init_info.conf.core = &core_conf;
        init_info.conf.video = &video_conf;
        init_info.conf.audio = &audio_conf;

        result_e result = kernel_init(&init_info);
        check_result(result, "could not init kernel");

        event_bus_register_handler(EVENT_WINDOW_CLOSE, on_window_close);
        event_bus_register_handler(EVENT_KBD_KEY_DOWN, on_keyboard_key_down);
    }

    ////////////////////////////////////////

    // init ui
    {
        struct ui_conf ui_conf = {0};
        ui_conf_defaults(&ui_conf);

        struct ui_init_info ui_info = {0};
        ui_info.imgui_ini_file = "/tmp/imgui.ini";
        ui_info.conf = &ui_conf;
        ui_info.ui_tick_cb = on_ui_tick;

        result_e result = ui_init(&ui_info);
        check_result(result, "could not init ui manager");
    }

    ////////////////////////////////////////

    kernel_run();

    ui_quit();
    kernel_quit();

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
