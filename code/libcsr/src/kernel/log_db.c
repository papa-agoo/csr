////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/vector.h>
#include <csr/kernel/log_db.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct log_db
{
    struct string name;

    struct vector *messages;
};

struct log_db* log_db_create(struct log_db_create_info *info)
{
    check_ptr(info);
    check_expr(info->max_messages > 0);

    struct log_db *db = calloc(1, sizeof(struct log_db));
    check_mem(db);

    db->name = string_is_valid(info->name) ? info->name : make_string("<no name>");

    db->messages = vector_create(info->max_messages, sizeof(struct log_message));
    check_mem(db->messages);

    return db;

error:
    if (db) {
        free(db);
    }

    return NULL;
}

void log_db_destroy(struct log_db *db)
{
    check_ptr(db);

    vector_destroy(db->messages);

    free(db);

error:
    return;
}

void log_db_flush_messages(struct log_db *db)
{
    check_ptr(db);

    vector_reset(db->messages);

error:
    return;
}

u32 log_db_get_message_count(struct log_db *db)
{
    check_ptr(db);

    return vector_size(db->messages);

error:
    return 0;
}

void log_db_add_message(struct log_db *db, struct log_message *message)
{
    check_ptr(db);
    check_ptr(message);

    vector_push_back(db->messages, *message);

error:
    return;
}

void log_db_traverse(struct log_db *db, log_db_traverse_cb_t callback, void *data)
{
    check_ptr(db);
    check_ptr(callback);

    for (u32 i = 0; i < vector_size(db->messages); i++)
    {
        struct log_message *message = vector_get(db->messages, i);

        callback(message, data);
    }

error:
    return;
}
