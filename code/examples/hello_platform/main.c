////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/platform.h>

#include <epoxy/gl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static bool g_app_running = true;

static void on_window_close()
{
    g_app_running = false;
}

result_e main()
{
    clog_info("platform init ...");

    struct platform_init_info init_info = {0};
    init_info.title = make_string("Hello Platform");
    init_info.width = 640;
    init_info.height = 480;

    check_result(platform_init(&init_info));

    platform_get_callbacks()->on_window_close = on_window_close;

    ////////////////////////////////////////

    const struct platform_backend_info *backend = &platform_get_info()->backend;

    clog_info(" - os backend : %S v%d.%d.%d", &backend->name,
        backend->version.major, backend->version.minor, backend->version.micro);

    const struct platform_renderer_info *renderer = &platform_get_info()->renderer;

    clog_info(" - render api : %S v%d.%d.%d", &renderer->name,
        renderer->version.major, renderer->version.minor, renderer->version.micro);

    ////////////////////////////////////////

    clog_info("running mainloop ...");

    while (g_app_running)
    {
        platform_poll_events();

        // clear framebuffer with blue color :)
        glClearColor(0, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        platform_swap_buffers();
    }

    ////////////////////////////////////////

    clog_info("platform quit ...");

    platform_quit();

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
