////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/file_io.h>
#include <csr/applet/applet_plugin.h>

// >>> FIXME
#include <dlfcn.h> // use file_io
#include <dirent.h> // use file_io
#include <csr/core/memory/arena_priv.h> // use scratch arena
// <<< FIXME

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct string _plugin_get_name() { return make_string("Stub"); }
static struct string _plugin_get_description() { return make_string("None"); }
static struct version _plugin_get_version() { return make_version(0, 0, 0); }

static result_e _plugin_startup() { return RC_SUCCESS; }
static void _plugin_shutdown() {}
static void _plugin_tick() {}

static void _init_plugin_defaults(struct applet_plugin* plugin)
{
    check_ptr(plugin);

    plugin->filename = make_string("<unknown>");

    plugin->get_name = _plugin_get_name;
    plugin->get_description = _plugin_get_description;
    plugin->get_version = _plugin_get_version;

    plugin->startup = _plugin_startup;
    plugin->shutdown = _plugin_shutdown;
    plugin->tick = _plugin_tick;

    plugin->handle = NULL;

error:
    return;
}

////////////////////////////////////////////////////////////

result_e applet_plugin_load(struct applet_plugin *plugin, struct string path_to_file)
{
    check_ptr(plugin);
    check_expr(string_is_valid(path_to_file));

    _init_plugin_defaults(plugin);

    plugin->filename = fio_fs_get_file_name(path_to_file);

    ////////////////////////////////////////

    // FIXME use scratch arena
    string_cstr path_cstr = cstr_from_string(_arena_priv_ptr(), path_to_file);

    // open library
    plugin->handle = dlopen(path_cstr, RTLD_NOW);
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
