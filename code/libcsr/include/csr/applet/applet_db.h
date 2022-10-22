////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct applet_db;

struct applet_db_entry
{
    const char* name;
    const char* description;
    const char* version_str;
    const char* filename;
};

struct applet_db *applet_db_create(const char* scan_path);
void applet_db_destroy(struct applet_db *db);

void applet_db_update(struct applet_db *db);

const char* applet_db_get_scan_path(struct applet_db *db);

u32 applet_db_get_entry_count(struct applet_db *db);

typedef void (*applet_db_traverse_cb_t)(struct applet_db_entry *entry, void *data);
void applet_db_traverse(struct applet_db *db, applet_db_traverse_cb_t callback, void *data);
