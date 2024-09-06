////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/vector.h>

#include <csr/applet/applet_db.h>
#include <csr/applet/applet_plugin.h>

#ifndef __USE_XOPEN2K8 
#define __USE_XOPEN2K8 // alphasort
#endif

#include <dirent.h> // FIXME fio_traverse_tree()

////////////////////////////////////////////////////////////////////////////////////////////////////

static s32 _name_filter(const struct dirent* entry)
{
    return (strcmp(entry->d_name, ".so") > 0);
}

static void _scan_applet_path(struct arena* arena, struct string path, struct vector* db)
{
    string_cstr path_cstr = cstr_from_string(arena, path);

    struct dirent** name_list;

    s32 name_count = scandir(path_cstr, &name_list, _name_filter, alphasort);

    for (s32 i = 0; i < name_count; i++)
    {
        // build relative path to the applet
        string_cstr filename_cstr = name_list[i]->d_name;

        struct string my_path = string_create_fmt(arena, "%s/%s", path_cstr, filename_cstr);

        // load plugin to access some info
        struct applet_plugin plugin = {0};

        result_e result = applet_plugin_load(&plugin, my_path);
        if (!R_SUCCESS(result)) continue;

        // create db entry and add to the db
        struct applet_db_entry entry = {0};
        entry.name = string_copy(arena, plugin.get_name());
        entry.description = string_copy(arena, plugin.get_description());
        entry.version_str = version_str(plugin.get_version());
        entry.filename = string_copy(arena, plugin.filename);

        vector_push_back(db, entry);

        // plugin not needed anymore
        applet_plugin_unload(&plugin);
    }

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

struct applet_db
{
    struct string scan_path;

    struct vector *entries;
};

struct applet_db *applet_db_create(struct arena* arena, struct string scan_path)
{
    check_expr(string_is_valid(scan_path));

    struct applet_db *db = calloc(1, sizeof(struct applet_db));
    check_mem(db);

    db->scan_path = scan_path;

    db->entries = vector_create(1, sizeof(struct applet_db_entry));
    check_ptr(db->entries);

    applet_db_update(arena, db);

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

void applet_db_update(struct arena* arena, struct applet_db *db)
{
    check_ptr(db);
    check_expr(string_is_valid(db->scan_path));
    check_ptr(db->entries);

    vector_reset(db->entries);

    _scan_applet_path(arena, db->scan_path, db->entries);

error:
    return;
}

struct string applet_db_get_scan_path(struct applet_db *db)
{
    check_ptr(db);

    return db->scan_path;

error:
    return make_string("");
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
