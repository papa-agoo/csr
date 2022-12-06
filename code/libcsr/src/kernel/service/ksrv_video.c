////////////////////////////////////////////////////////////////////////////////////////////////////

#define KLOG_MODULE_NAME kernel

#include <csr/kernel.h>
#include <csr/kernel/kernel_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _on_window_resize(struct event* event)
{
    struct ksrv_video *srv = ksrv_video_ptr();

    ////////////////////////////////////////

    struct vec2 resolution = make_vec2(event->window.width, event->window.height);
    klog_trace("_on_window_resize() : %d, %d", (u32)resolution.w, (u32)resolution.h);

    // FIXME ignore resolution change if going fullscreen ?
    srv->conf->window.video_mode = video_mode_create(resolution);

    ////////////////////////////////////////

    // update swapchain
    ksrv_video_create_swapchain((u32)resolution.w, (u32)resolution.h, &srv->swapchain);
}

result_e ksrv_video_init(struct ksrv_video_init_info *init_info)
{
    check_ptr(init_info);
    check_ptr(init_info->conf);

    klog_info("initializing video service ...");

    ////////////////////////////////////////

    struct ksrv_video *srv = ksrv_video_ptr();
    srv->conf = init_info->conf;

    result_e result = xgl_init_driver();
    check_result(result, "could not init xgl driver");

    struct xgl_driver_info driver_info = {0};
    xgl_get_driver_info(&driver_info);

    check_expr(driver_info.type != XGL_DRIVER_TYPE_UNKNOWN);

    // FIXME check : platform renderer VS xgl driver type (OPENGL, ...)

    struct version v = driver_info.version;

    klog_info(" - xgl driver : %s v%d.%d.%d", driver_info.name, v.major, v.minor, v.micro);

    // FIXME check : platform_get_platform_info()->renderer.version >= driver_info.version

    ////////////////////////////////////////

    struct video_mode *vm = &srv->conf->window.video_mode;

    result = ksrv_video_create_swapchain((u32)vm->resolution.w, (u32)vm->resolution.h, &srv->swapchain);
    check_result(result, "could not create swapchain");

    ////////////////////////////////////////

    event_bus_register_handler(EVENT_WINDOW_RESIZE, _on_window_resize);

    ////////////////////////////////////////

    srv->is_initialized = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void ksrv_video_quit()
{
    klog_info("quitting video service ...");

    ////////////////////////////////////////

    struct ksrv_video *srv = ksrv_video_ptr();

    xgl_destroy_swapchain(srv->swapchain);
    xgl_quit_driver();

    ////////////////////////////////////////

    srv->is_initialized = false;
}

void ksrv_video_prepare_framebuffer()
{
    struct ksrv_video *srv = ksrv_video_ptr();

    // FIXME explicit buffer swap?
}

void ksrv_video_present_framebuffer()
{
    struct ksrv_video *srv = ksrv_video_ptr();

    xgl_present_swapchain_framebuffer(srv->swapchain);
}

result_e ksrv_video_create_swapchain(u32 width, u32 height, xgl_swapchain *p_swapchain)
{
    xgl_swapchain swapchain;
    {
        struct xgl_swapchain_create_info create_info = {0};
        create_info.width = width;
        create_info.height = height;

        result_e result = xgl_create_swapchain(&create_info, &swapchain);
        check_result(result, "could not create swapchain");
    }

    // free resources of the old swapchain
    if (guid_valid(p_swapchain->handle)) {
        xgl_destroy_swapchain(*p_swapchain);
    }

    // set current swapchain
    *p_swapchain = swapchain;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
