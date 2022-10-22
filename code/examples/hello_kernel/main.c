////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/kernel.h>

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

static void on_kernel_tick()
{
    struct xgl_render_pass_info pass_info = {0};
    pass_info.name = "My Pass";
    pass_info.clear_values.color = make_vec4(0, 0, 1, 1);
    pass_info.framebuffer = kio_video_get_framebuffer();

    if (xgl_begin_render_pass(&pass_info))
    {
        {
            // render commands ...
        }

        xgl_end_render_pass();
    }
}

result_e main()
{
    ////////////////////////////////////////

    struct ksrv_core_conf core_conf = {0};
    ksrv_core_conf_defaults(&core_conf);

    struct ksrv_video_conf video_conf = {0};
    ksrv_video_conf_defaults(&video_conf);

    struct ksrv_audio_conf audio_conf = {0};
    ksrv_audio_conf_defaults(&audio_conf);

    // registry, vfs, ...

    ////////////////////////////////////////

    struct kernel_init_info init_info = {0};
    init_info.name = "Hello Kernel";
    init_info.kernel_tick_cb = on_kernel_tick;

    init_info.conf.core = &core_conf;
    init_info.conf.video = &video_conf;
    init_info.conf.audio = &audio_conf;

    result_e result = kernel_init(&init_info);
    check_result(result, "could not init kernel");

    event_bus_register_handler(EVENT_WINDOW_CLOSE, on_window_close);
    event_bus_register_handler(EVENT_KBD_KEY_DOWN, on_keyboard_key_down);

    kernel_run();
    kernel_quit();

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
