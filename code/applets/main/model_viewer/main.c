////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define RGPU_SCREEN_WIDTH   1280
#define RGPU_SCREEN_HEIGHT  720

#define RCPU_SCREEN_WIDTH   640
#define RCPU_SCREEN_HEIGHT  360

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct string _plugin_get_name()
{
    return make_string("Model Viewer");
}

static struct string _plugin_get_description()
{
    return make_string("Model Viewer for obj / gltf / native 3D Models");
}

static struct version _plugin_get_version()
{
    return make_version(0, 0, 1);
}

static result_e _plugin_startup()
{
    // create screens
    struct screen *screen_rgpu;
    {
        struct screen_create_info create_info = {0};
        create_info.name = "GPU Renderer (XGL OpenGL)";

        create_info.surface.viewport.width = RGPU_SCREEN_WIDTH;
        create_info.surface.viewport.height = RGPU_SCREEN_HEIGHT;
        create_info.surface.clear_values.color = make_vec4(0, 0, 1, 1);

        screen_rgpu = aio_add_screen("rgpu", &create_info);
        check_ptr(screen_rgpu);
    }

    struct screen *screen_rcpu;
    {
        struct screen_create_info create_info = {0};
        create_info.name = "CPU Renderer (SoftGL)";

        create_info.surface.type = SCREEN_SURFACE_TYPE_CPU;
        create_info.surface.viewport.width = RCPU_SCREEN_WIDTH;
        create_info.surface.viewport.height = RCPU_SCREEN_HEIGHT;
        create_info.surface.clear_values.color = make_vec4(1, 0, 0, 1);

        screen_rcpu = aio_add_screen("rcpu", &create_info);
        check_ptr(screen_rcpu);
    }

    ////////////////////////////////////////

    // set hid callbacks
    struct hid_callbacks *hid = aio_get_hid_callbacks();
    check_ptr(hid);

    // ...

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _plugin_shutdown()
{
    // ...
}

static void _plugin_tick()
{
    if (screen_begin(aio_get_screen("rgpu"), SCREEN_SURFACE_TYPE_GPU))
    {
        // ...

        screen_end();
    }

    if (screen_begin(aio_get_screen("rcpu"), SCREEN_SURFACE_TYPE_CPU))
    {
        // ...

        screen_end();
    }
}

void applet_plugin_export(struct applet_plugin* plugin)
{
    plugin->get_name = _plugin_get_name;
    plugin->get_description = _plugin_get_description;
    plugin->get_version = _plugin_get_version;

    plugin->startup = _plugin_startup;
    plugin->shutdown = _plugin_shutdown;
    plugin->tick = _plugin_tick;
}
