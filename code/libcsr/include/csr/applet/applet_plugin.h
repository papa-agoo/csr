////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct applet_plugin
{
    const char *filename;

    const char* (*get_name)();
    const char* (*get_description)();
    struct version (*get_version)();

    result_e (*startup)();
    void (*shutdown)();
    void (*tick)();

    void* handle;
};

result_e applet_plugin_load(struct applet_plugin *plugin, const char *path_to_file);
void applet_plugin_unload(struct applet_plugin* plugin);

typedef void (*applet_plugin_export_cb_t)(struct applet_plugin* plugin);
void applet_plugin_export(struct applet_plugin* plugin);
