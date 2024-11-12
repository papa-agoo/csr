////////////////////////////////////////////////////////////////////////////////////////////////////

#define KLOG_MODULE_NAME kernel

#include <csr/kernel.h>
#include <csr/kernel/kernel_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct kernel g_kernel = {0};

struct kernel* kernel_ptr()
{
    return &g_kernel;
}

////////////////////////////////////////////////////////////////////////////////

result_e kernel_init(struct kernel_init_info *init_info)
{
    csr_assert(!kernel_ptr()->is_initialized);

    check_ptr(init_info);

    check_ptr(init_info->conf.core);
    check_ptr(init_info->conf.video);

    result_e result = RC_FAILURE;

    ////////////////////////////////////////

    struct platform_init_info platform_info = {0};
    platform_info.title = init_info->name;
    platform_info.width = init_info->conf.video->window.video_mode.resolution.w;
    platform_info.height = init_info->conf.video->window.video_mode.resolution.h;
    platform_info.fullscreen = init_info->conf.video->window.fullscreen;
    platform_info.enable_vsync = init_info->conf.video->window.vsync;
    platform_info.hint.sample_count = init_info->conf.video->xgl.msaa_count;
    platform_info.hint.debug_context = init_info->conf.video->xgl.debug_mode;

    struct ksrv_core_init_info core_info = {0};
    core_info.conf = init_info->conf.core;
    core_info.kernel_tick_cb = init_info->kernel_tick_cb;
    core_info.platform_info = &platform_info;

    check_result(ksrv_core_init(&core_info));

    ////////////////////////////////////////

    struct ksrv_video_init_info video_info = {0};
    video_info.conf = init_info->conf.video;

    check_result(ksrv_video_init(&video_info));

    ////////////////////////////////////////

    kernel_ptr()->is_initialized = true;

    return RC_SUCCESS;

error:

    if (ksrv_video_ptr()->is_initialized) {
        ksrv_video_quit();
    }

    if (ksrv_core_ptr()->is_initialized) {
        ksrv_core_quit();
    }

    return RC_FAILURE;
}

void kernel_quit()
{
    csr_assert(kernel_ptr()->is_initialized);

    if (kernel_ptr()->is_running)
    {
        klog_warn("kernel_quit() : kernel still running!? ... stopping now!");

        kernel_stop();
    }

    ksrv_video_quit();
    ksrv_core_quit();
}

void kernel_run()
{
    csr_assert(kernel_ptr()->is_initialized);

    kernel_ptr()->is_running = true;

    ////////////////////////////////////////

    // ksrv_core_mainloop_run();
    {
        struct ksrv_core *core_srv = ksrv_core_ptr();

        klog_info("running mainloop ...");

        // init clock
        f64 current_time = kio_time_elapsed();
        clock_init(core_srv->clock, current_time);

        // run mainloop
        mainloop_run(core_srv->mainloop);

        ////////////////////////////////////////

        struct frame_stats* stats = &core_srv->frame_stats;

        klog_info("mainloop was running for %.03fs", stats->runtime_s);
        klog_info(" - num frames : %d", stats->num_frames);
        klog_info(" - average fps : %.2f", stats->avg_fps);
        klog_info(" - average frametime : %.2fms", stats->avg_frametime_ms);
    }
}

void kernel_stop()
{
    csr_assert(kernel_ptr()->is_running);

    // ksrv_core_mainloop_stop();
    {
        klog_info("stopping mainloop ...");

        mainloop_exit(ksrv_core_ptr()->mainloop);
    }

    kernel_ptr()->is_running = false;
}
