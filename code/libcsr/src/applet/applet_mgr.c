////////////////////////////////////////////////////////////////////////////////////////////////////

#define KLOG_MODULE_NAME applet_mgr
#include <csr/kernel/kio.h>

#include <csr/core/file_io.h>
#include <csr/applet/applet_mgr.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct applet_mgr
{
    bool is_initialized;

    bool needs_applet_reload; // FIXME message system
    bool needs_applet_unload; // FIXME message system

    struct applet *applet;
    struct applet_db *applet_db;

    struct applet_mgr_callbacks callbacks;
    struct applet_mgr_conf *conf;
};

static struct applet_mgr g_mgr = {0};

#define mgr_ptr() (&g_mgr)

////////////////////////////////////////////////////////////

result_e applet_mgr_init(struct applet_mgr_init_info *init_info)
{
    csr_assert(!mgr_ptr()->is_initialized);

    check_ptr(init_info);
    check_ptr(init_info->conf);
    check_expr(string_is_valid(init_info->db_scan_path));

    ////////////////////////////////////////

    struct arena* arena = kio_mem_get_main_arena();
    check_ptr(arena);

    mgr_ptr()->applet_db = applet_db_create(arena, init_info->db_scan_path);
    check_ptr(mgr_ptr()->applet_db);

    mgr_ptr()->callbacks = init_info->callbacks;
    mgr_ptr()->conf = init_info->conf;

    mgr_ptr()->is_initialized = true;

    ////////////////////////////////////////

    // update applet db
    applet_mgr_update_db();

    // load last applet
    struct applet_mgr_conf *conf = mgr_ptr()->conf;

    if (conf->remember_applet)
    {
        struct string applet_name = make_string_from_cstr(conf->applet_name);

        if (string_is_valid(applet_name)) {
            applet_mgr_load_applet(applet_name);
        }
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void applet_mgr_quit()
{
    csr_assert(mgr_ptr()->is_initialized);

    applet_db_destroy(mgr_ptr()->applet_db);

    applet_mgr_unload_applet();

    mgr_ptr()->is_initialized = false;
}

void applet_mgr_tick()
{
    csr_assert(mgr_ptr()->is_initialized);

    ////////////////////////////////////////

    if (mgr_ptr()->needs_applet_reload) {
        applet_mgr_reload_applet();
    }

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

    struct arena* arena = kio_mem_get_main_arena();
    check_ptr(arena);

    struct applet_db *applet_db = mgr_ptr()->applet_db;
    applet_db_update(arena, applet_db);

    klog_info("  - applets found : %d", applet_db_get_entry_count(applet_db));

error:
    return;
}

bool applet_mgr_is_applet_loaded()
{
    return mgr_ptr()->applet != NULL;
}

result_e applet_mgr_load_applet(struct string filename)
{
    csr_assert(mgr_ptr()->is_initialized);

    check_expr(string_is_valid(filename));

    // if an applet is already loaded, unload it first
    applet_mgr_unload_applet();

    ////////////////////////////////////////

    klog_notice("loading applet : "string_fmt, string_fmt_arg(filename));

    // build applet path
    struct arena* arena = kio_mem_get_main_arena();
    check_ptr(arena);

    struct string db_scan_path = applet_db_get_scan_path(mgr_ptr()->applet_db);

    struct string path_to_file = string_create_fmt(arena,
        string_fmt"/"string_fmt, string_fmt_arg(db_scan_path), string_fmt_arg(filename));
    
    // load applet
    check_expr(fio_fs_is_file(path_to_file));

    struct applet *applet = applet_create(path_to_file);
    check(applet != NULL, "could not load applet : "string_fmt, string_fmt_arg(path_to_file));

    mgr_ptr()->applet = applet;

    // run post load callback(s)
    if (mgr_ptr()->callbacks.on_post_applet_load) {
        mgr_ptr()->callbacks.on_post_applet_load(applet);
    }

    ////////////////////////////////////////

    klog_trace("starting applet ...");

    result_e result = applet_startup(applet);

    if (R_FAILURE(result))
    {
        klog_error("applet->startup() failed ("string_fmt") ...", string_fmt_arg(applet_get_filename(applet)));

        applet_mgr_unload_applet();

        return RC_FAILURE;
    }

    klog_trace("applet started");

    ////////////////////////////////////////

    // remember started applet
    if (mgr_ptr()->conf->remember_applet) {
        mgr_ptr()->conf->applet_name = string_get_cstr(arena, filename);
    }

    return RC_SUCCESS;

error:
    klog_error("something went wrong ... see terminal output :(");

    applet_mgr_unload_applet();

    return RC_FAILURE;

}

void applet_mgr_reload_applet()
{
    csr_assert(mgr_ptr()->is_initialized);

    struct applet *applet = mgr_ptr()->applet;
    check_ptr(applet);

    applet_mgr_load_applet(applet_get_filename(applet));

error:
    mgr_ptr()->needs_applet_reload = false;

    return;
}

void applet_mgr_unload_applet()
{
    csr_assert(mgr_ptr()->is_initialized);

    struct applet *applet = mgr_ptr()->applet;
    check_quiet(applet);

    ////////////////////////////////////////

    klog_trace("stopping applet ...");

    applet_shutdown(applet);

    klog_trace("applet stopped");

    ////////////////////////////////////////

    if (mgr_ptr()->callbacks.on_pre_applet_unload) {
        mgr_ptr()->callbacks.on_pre_applet_unload(applet);
    }

    klog_notice("unloading applet : "string_fmt, string_fmt_arg(applet_get_filename(applet)));

    applet_destroy(applet);

    mgr_ptr()->applet = NULL;

error:
    mgr_ptr()->needs_applet_unload = false;

    return;
}

void applet_mgr_request_applet_reload()
{
    csr_assert(mgr_ptr()->is_initialized);

    mgr_ptr()->needs_applet_reload = true;
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
