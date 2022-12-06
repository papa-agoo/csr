////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum log_level_type
{
    LOG_LEVEL_INFO,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_TRACE,

    LOG_LEVEL_MAX,
};

struct log_message
{
    u32 frame;
    u32 frame_time_ms;

    const char* module;
    const char* message;

    enum log_level_type level;
};

struct log_db_create_info
{
    const char *name;

    u32 max_messages;
};

struct log_db;

struct log_db* log_db_create(struct log_db_create_info *info);
void log_db_destroy(struct log_db *db);

void log_db_flush_messages(struct log_db *db);
u32 log_db_get_message_count(struct log_db *db);

void log_db_add_message(struct log_db *db, struct log_message *message);

typedef void (*log_db_traverse_cb_t)(struct log_message *message, void *data);
void log_db_traverse(struct log_db *db, log_db_traverse_cb_t callback, void *data);
