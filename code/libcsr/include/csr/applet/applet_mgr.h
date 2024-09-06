////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/applet/applet.h>
#include <csr/applet/applet_db.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct applet_mgr_callbacks
{
    void (*on_post_applet_load)(struct applet *applet);
    void (*on_pre_applet_unload)(struct applet *applet);
};

struct applet_mgr_init_info
{
    struct string db_scan_path;

    struct applet_mgr_callbacks callbacks;
};

result_e applet_mgr_init(struct applet_mgr_init_info *init_info);
void applet_mgr_quit();

void applet_mgr_tick();

void applet_mgr_update_db();

bool applet_mgr_is_applet_loaded();
result_e applet_mgr_load_applet(struct string filename);
void applet_mgr_unload_applet();

void applet_mgr_request_applet_unload(); // FIXME need message system

struct applet* applet_mgr_get_applet();
struct applet_db* applet_mgr_get_applet_db();
