////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e model_viewer_register_ui();

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
    check_result(model_viewer_init(), "could not init model viewer");
    check_result(model_viewer_register_ui(), "could not register ui");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _plugin_shutdown()
{
    model_viewer_quit();
}

static void _plugin_tick()
{
    model_viewer_tick();
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
