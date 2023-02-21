////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet.h>

#include <epoxy/gl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* _plugin_get_name()
{
    return "Hello Screens";
}

static const char* _plugin_get_description()
{
    return "Examples for Pixelbuffers and XGL / OpenGL Framebuffer usage";
}

static struct version _plugin_get_version()
{
    return make_version(1, 0, 0);
}

static result_e _plugin_startup()
{
    struct vec2 size = make_vec2(640, 480);

    // xgl framebuffer
    {
        struct screen_create_info create_info = {0};
        create_info.name = "XGL Framebuffer";

        create_info.surface.viewport.width = size.w;
        create_info.surface.viewport.height = size.h;
        create_info.surface.clear_values.color = make_vec4(0, 1, 0, 1);

        struct screen *screen = aio_add_screen("fb.xgl", &create_info);
        check_ptr(screen);
    }

    // opengl framebuffer
    {
        struct screen_create_info create_info = {0};
        create_info.name = "OpenGL Framebuffer";

        create_info.surface.viewport.width = size.w;
        create_info.surface.viewport.height = size.h;
        create_info.surface.clear_values.color = make_vec4(0, 0, 1, 1);

        struct screen *screen = aio_add_screen("fb.ogl", &create_info);
        check_ptr(screen);
    }

    // pixelbuffer
    {
        struct screen_create_info create_info = {0};
        create_info.name = "Pixelbuffer";

        create_info.surface.type = SCREEN_SURFACE_TYPE_CPU;
        create_info.surface.viewport.width = size.w;
        create_info.surface.viewport.height = size.h;
        create_info.surface.clear_values.color = make_vec4(1, 0, 0, 1);

        struct screen *screen = aio_add_screen("pb", &create_info);
        check_ptr(screen);
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _plugin_tick()
{
    // xgl framebuffer
    if (screen_begin(aio_get_screen("fb.xgl"), SCREEN_SURFACE_TYPE_GPU))
    {
        // ...

        screen_end();
    }

    ////////////////////////////////////////

    // opengl framebuffer
    if (screen_begin(aio_get_screen("fb.ogl"), SCREEN_SURFACE_TYPE_GPU))
    {
        glClearColor(0, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // ...

        screen_end();
    }

    ////////////////////////////////////////

    // pixelbuffer
    struct screen *screen_pb = aio_get_screen("pb");

    if (screen_begin(screen_pb, SCREEN_SURFACE_TYPE_CPU))
    {
        const struct pixelbuffer *pb = screen_get_pixelbuffer(screen_pb);

        if (pb)
        {
            // ...
        }

        screen_end();
    }
}

////////////////////////////////////////////////////////////////////////////////

void applet_plugin_export(struct applet_plugin* plugin)
{
    plugin->get_name = _plugin_get_name;
    plugin->get_description = _plugin_get_description;
    plugin->get_version = _plugin_get_version;

    plugin->startup = _plugin_startup;
    plugin->tick = _plugin_tick;
}
