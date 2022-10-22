////////////////////////////////////////////////////////////////////////////////////////////////////

#include <dlfcn.h>
#include <dirent.h>

#include <csr/core/path.h>

#include <csr/applet/applet_plugin.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* _plugin_get_name() { return "Stub"; }
static const char* _plugin_get_description() { return "None"; }
static struct version _plugin_get_version() { return make_version(0, 0, 0); }

static result_e _plugin_startup() { return RC_SUCCESS; }
static void _plugin_shutdown() {}
static void _plugin_iterate() {}

static void _init_plugin_defaults(struct applet_plugin* plugin)
{
    check_ptr(plugin);

    plugin->filename = "<unknown>";

    plugin->get_name = _plugin_get_name;
    plugin->get_description = _plugin_get_description;
    plugin->get_version = _plugin_get_version;

    plugin->startup = _plugin_startup;
    plugin->shutdown = _plugin_shutdown;
    plugin->iterate = _plugin_iterate;

    plugin->handle = NULL;

error:
    return;
}

////////////////////////////////////////////////////////////

result_e applet_plugin_load(struct applet_plugin *plugin, const char *path_to_file)
{
    check_ptr(plugin);
    check_ptr(path_to_file);

    _init_plugin_defaults(plugin);

    plugin->filename = path_get_basename(path_to_file);

    ////////////////////////////////////////

    // open library
    plugin->handle = dlopen(path_to_file, RTLD_NOW);
    check(plugin->handle != NULL, "%s", dlerror());

    // export plugin symbols
    applet_plugin_export_cb_t export_plugin_cb = dlsym(plugin->handle, "applet_plugin_export");
    check(export_plugin_cb != NULL, "%s", dlerror());

    export_plugin_cb(plugin);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    applet_plugin_unload(plugin);

    return RC_FAILURE;
}

void applet_plugin_unload(struct applet_plugin* plugin)
{
    check_ptr(plugin);

    if (plugin->handle) {
        dlclose(plugin->handle);
    }

    _init_plugin_defaults(plugin);

error:
    return;
}
