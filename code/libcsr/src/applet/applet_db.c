////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/path.h>
#include <csr/core/vector.h>

#include <csr/applet/applet_db.h>
#include <csr/applet/applet_plugin.h>

#ifndef __USE_XOPEN2K8 
#define __USE_XOPEN2K8 // alphasort
#endif

#include <dirent.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static s32 _name_filter(const struct dirent* entry)
{
    return (strcmp(entry->d_name, ".so") > 0);
}

static void _scan_applet_path(const char* dirname, struct vector* db)
{
    check_ptr(dirname);
    check_ptr(db);

    struct dirent** name_list;

    s32 name_count = scandir(dirname, &name_list, _name_filter, alphasort);

    for (s32 i = 0; i < name_count; i++)
    {
        const char* filename = name_list[i]->d_name;

        const char *path_to_file = path_merge(dirname, filename);

        struct applet_plugin plugin = {0};

        result_e result = applet_plugin_load(&plugin, path_to_file);
        if (!R_SUCCESS(result)) continue;

        ////////////////////////////////////////

        struct applet_db_entry entry = {0};
        entry.name = strdup(plugin.get_name());
        entry.description = strdup(plugin.get_description());
        entry.version_str = version_str(plugin.get_version());
        entry.filename = strdup(plugin.filename);

        vector_push_back(db, entry);

        ////////////////////////////////////////

        applet_plugin_unload(&plugin);
    }

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

struct applet_db
{
    const char *scan_path;

    struct vector *entries;
};

struct applet_db *applet_db_create(const char* scan_path)
{
    check_ptr(scan_path);

    struct applet_db *db = calloc(1, sizeof(struct applet_db));
    check_mem(db);

    db->scan_path = strdup(scan_path);

    db->entries = vector_create(1, sizeof(struct applet_db_entry));
    check_ptr(db->entries);

    applet_db_update(db);

    return db;

error:
    return NULL;
}

void applet_db_destroy(struct applet_db *db)
{
    check_ptr(db);

    vector_destroy(db->entries);

    free(db);

error:
    return;
}

void applet_db_update(struct applet_db* db)
{
    check_ptr(db);
    check_ptr(db->scan_path);
    check_ptr(db->entries);

    vector_reset(db->entries);

    _scan_applet_path(db->scan_path, db->entries);

error:
    return;
}

const char* applet_db_get_scan_path(struct applet_db *db)
{
    check_ptr(db);

    return db->scan_path;

error:
    return NULL;
}

u32 applet_db_get_entry_count(struct applet_db *db)
{
    check_ptr(db);

    return vector_size(db->entries);

error:
    return 0;
}

void applet_db_traverse(struct applet_db *db, applet_db_traverse_cb_t callback, void *data)
{
    check_ptr(db);
    check_ptr(callback);

    for (u32 i = 0; i < vector_size(db->entries); i++)
    {
        struct applet_db_entry *e = vector_get(db->entries, i);

        callback(e, data);
    }

error:
    return;
}
