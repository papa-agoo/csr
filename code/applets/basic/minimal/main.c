////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* _plugin_get_name()
{
    return "Minimal";
}

static const char* _plugin_get_description()
{
    return "Minimal Example (no ui, no rendering, nothing ...)";
}

static struct version _plugin_get_version()
{
    return make_version(1, 0, 0);
}

void applet_plugin_export(struct applet_plugin* plugin)
{
    // override the generic plugin stubs (optional)
    plugin->get_name = _plugin_get_name;
    plugin->get_description = _plugin_get_description;
    plugin->get_version = _plugin_get_version;
}
