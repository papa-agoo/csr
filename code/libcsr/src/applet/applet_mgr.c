////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/path.h>

#define KLOG_MODULE_NAME applet_mgr
#include <csr/kernel/kio.h>

#include <csr/applet/applet_mgr.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct applet_mgr
{
    bool is_initialized;
    bool needs_applet_unload; // FIXME message system

    struct applet *applet;
    struct applet_db *applet_db;

    struct applet_mgr_callbacks callbacks;
};

static struct applet_mgr g_mgr = {0};

#define mgr_ptr() (&g_mgr)

////////////////////////////////////////////////////////////

result_e applet_mgr_init(struct applet_mgr_init_info *init_info)
{
    csr_assert(!mgr_ptr()->is_initialized);

    check_ptr(init_info);
    check_ptr(init_info->db_scan_path);

    ////////////////////////////////////////

    mgr_ptr()->applet_db = applet_db_create(init_info->db_scan_path);
    check_ptr(mgr_ptr()->applet_db);

    mgr_ptr()->callbacks = init_info->callbacks;

    mgr_ptr()->is_initialized = true;

    ////////////////////////////////////////

    applet_mgr_update_db();

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void applet_mgr_quit()
{
    csr_assert(mgr_ptr()->is_initialized);

    applet_db_destroy(mgr_ptr()->applet_db);

    if (mgr_ptr()->applet) {
        applet_mgr_unload_applet();
    }

    mgr_ptr()->is_initialized = false;
}

void applet_mgr_tick()
{
    csr_assert(mgr_ptr()->is_initialized);

    ////////////////////////////////////////

    if (mgr_ptr()->needs_applet_unload) {
        applet_mgr_unload_applet();
    }

    ////////////////////////////////////////

    struct applet *applet = mgr_ptr()->applet;

    if (applet) {
        applet_tick(applet, kio_time_elapsed_delta());
    }
}

void applet_mgr_update_db()
{
    csr_assert(mgr_ptr()->is_initialized);

    klog_info("updating applet db ...");

    struct applet_db *applet_db = mgr_ptr()->applet_db;
    applet_db_update(applet_db);

    klog_info("  - applets found : %d", applet_db_get_entry_count(applet_db));
}

bool applet_mgr_is_applet_loaded()
{
    return mgr_ptr()->applet != NULL;
}

result_e applet_mgr_load_applet(const char* filename)
{
    csr_assert(mgr_ptr()->is_initialized);

    check_ptr(filename);

    ////////////////////////////////////////

    if (mgr_ptr()->applet) {
        applet_mgr_unload_applet();
    }

    klog_notice("loading applet : %s", filename);

    const char *db_scan_path = applet_db_get_scan_path(mgr_ptr()->applet_db);
    const char *path_to_file = path_merge(db_scan_path, filename); // FIXME improve path api

    // load applet
    struct applet *applet = applet_create(path_to_file);
    check(applet != NULL, "could not load applet : %s", path_to_file);

    mgr_ptr()->applet = applet;

    if (mgr_ptr()->callbacks.on_post_applet_load) {
        mgr_ptr()->callbacks.on_post_applet_load(applet);
    }

    ////////////////////////////////////////

    klog_trace("starting applet ...");

    result_e result = applet_startup(applet);

    if (R_FAILURE(result))
    {
        klog_error("applet->startup() failed (%s) ...", applet_get_filename(applet));

        applet_mgr_unload_applet();

        return RC_FAILURE;
    }

    klog_trace("applet started");

    return RC_SUCCESS;

error:
    applet_mgr_unload_applet();

    return RC_FAILURE;

}

void applet_mgr_unload_applet()
{
    csr_assert(mgr_ptr()->is_initialized);

    struct applet *applet = mgr_ptr()->applet;
    check_ptr(applet);

    ////////////////////////////////////////

    klog_trace("stopping applet ...");

    applet_shutdown(applet);

    klog_trace("applet stopped");

    ////////////////////////////////////////

    if (mgr_ptr()->callbacks.on_pre_applet_unload) {
        mgr_ptr()->callbacks.on_pre_applet_unload(applet);
    }

    klog_notice("unloading applet : %s", applet_get_filename(applet));

    applet_destroy(applet);

    mgr_ptr()->applet = NULL;

error:
    mgr_ptr()->needs_applet_unload = false;

    return;
   
}

void applet_mgr_request_applet_unload()
{
    csr_assert(mgr_ptr()->is_initialized);

    mgr_ptr()->needs_applet_unload = true;
}

struct applet* applet_mgr_get_applet()
{
    csr_assert(mgr_ptr()->is_initialized);

    return mgr_ptr()->applet;
}

struct applet_db* applet_mgr_get_applet_db()
{
    csr_assert(mgr_ptr()->is_initialized);

    return mgr_ptr()->applet_db;
}
