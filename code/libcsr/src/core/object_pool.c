////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/object_pool.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct object_pool
{
    const char *name;

    size_t object_size;
    size_t object_chunk_size;

    u32 object_count;
    u32 object_count_max;
    u32 object_count_per_chunk;

    u32 **index_chunks;
    u32 **magic_chunks;
    void **object_chunks;
};

static result_e _resize_chunks(struct object_pool* pool)
{
    check_ptr(pool);

    u32 chunk_count = pool->object_count / pool->object_count_per_chunk;

    ////////////////////////////////////////

    size_t index_chunks_size = (chunk_count + 1) * (sizeof(u32**) * pool->object_count_per_chunk);

    pool->index_chunks = realloc(pool->index_chunks, index_chunks_size);
    check_mem(pool->index_chunks);

    pool->index_chunks[chunk_count] = malloc(sizeof(u32*) * pool->object_count_per_chunk);
    check_mem(pool->index_chunks[chunk_count]);

    ////////////////////////////////////////

    size_t magic_chunks_size = index_chunks_size;

    pool->magic_chunks = realloc(pool->magic_chunks, magic_chunks_size);
    check_mem(pool->magic_chunks);

    pool->magic_chunks[chunk_count] = malloc(sizeof(u32*) * pool->object_count_per_chunk);
    check_mem(pool->magic_chunks[chunk_count]);

    ////////////////////////////////////////

    size_t object_chunks_size = (chunk_count + 1) * pool->object_chunk_size;

    pool->object_chunks = realloc(pool->object_chunks, object_chunks_size);
    check_mem(pool->object_chunks);

    pool->object_chunks[chunk_count] = malloc(pool->object_chunk_size);
    check_mem(pool->object_chunks[chunk_count]);

    ////////////////////////////////////////

    for (u32 i = 0; i < pool->object_count_per_chunk; i++)
    {
        pool->index_chunks[chunk_count][i] = pool->object_count + i;
        pool->magic_chunks[chunk_count][i] = 0xFFFFFFFF;
    }

    pool->object_count_max += pool->object_count_per_chunk;

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

struct object_pool* object_pool_create(const char* name, size_t object_size)
{
    check_ptr(name);
    check_expr(object_size > 0);

    ////////////////////////////////////////

    struct object_pool *pool = calloc(1, sizeof(struct object_pool));
    check_mem(pool);

    pool->name = strdup(name);

    pool->object_size = object_size;
    pool->object_chunk_size = 4096;

    pool->object_count = 0;
    pool->object_count_max = 0;
    pool->object_count_per_chunk = pool->object_chunk_size / pool->object_size;

    pool->index_chunks = NULL;
    pool->magic_chunks = NULL;
    pool->object_chunks = NULL;

    ////////////////////////////////////////

    return pool;

error:
    return NULL;
}

void object_pool_destroy(struct object_pool *pool)
{
    check_ptr(pool);

    check(pool->object_count == 0, "object_pool_destroy( %s ) : %u / %u slots still allocated !?",
        pool->name, pool->object_count, pool->object_count_max);

    ////////////////////////////////////////

    u32 chunk_count = pool->object_count / pool->object_count_per_chunk;

    for (u32 i = 0; i < chunk_count; i++)
    {
        free(pool->index_chunks[i]);
        free(pool->magic_chunks[i]);
        free(pool->object_chunks[i]);
    }

    free(pool->index_chunks);
    free(pool->magic_chunks);
    free(pool->object_chunks);

    ////////////////////////////////////////

    *pool = (struct object_pool) {0};

error:
    return;
}

guid object_pool_alloc(struct object_pool* pool, void* object)
{
    check_ptr(pool);
    check_ptr(object);

    if (pool->object_count == pool->object_count_max)
    {
        result_e result = _resize_chunks(pool);
        check(R_SUCCESS(result), "_resize_chunks() failed");
    }

    ////////////////////////////////////////

    // _read_index_from_object_count()
    u32 chunk_idx = pool->object_count / pool->object_count_per_chunk;
    u32 object_idx = pool->object_count % pool->object_count_per_chunk;

    u32 index = pool->index_chunks[chunk_idx][object_idx];

    guid id = guid_create(index);

    u32 magic = guid_get_magic(id);

    // _write_magic_from_index()
    chunk_idx = index / pool->object_count_per_chunk;
    object_idx = index % pool->object_count_per_chunk;

    pool->magic_chunks[chunk_idx][object_idx] = magic;

    pool->object_count++;

    ////////////////////////////////////////

    void *slot_ptr = pool->object_chunks[chunk_idx] + (object_idx * pool->object_size);
    memcpy(slot_ptr, object, pool->object_size);

    ////////////////////////////////////////
#ifdef CSR_TRACE_OBJECT_POOL
    clog_trace(">>> object_pool_alloc( %s ) : guid = %lu, magic = %u, index = %u [ chunk: %u, object: %u ]",
        pool->name, id.id, magic, index, chunk_idx, object_idx);

    size_t byte_size_used = pool->object_count * pool->object_size;
    size_t byte_size_total = pool->object_count_max * pool->object_size;

    clog_trace("  = slots used : %u/%u ( %lu/%lu bytes )",
        pool->object_count, pool->object_count_max, byte_size_used, byte_size_total);
#endif
    ////////////////////////////////////////

    return id;

error:
    return make_guid(0);
}

void object_pool_free(struct object_pool* pool, guid object)
{
    check_ptr(pool);
    check_expr(pool->object_count > 0);

    u32 index = guid_get_value(object);
    check(index < pool->object_count_max, "[ %s ] guid invalid (bad index) : %u", pool->name, index);

    // _read_magic_from_index()
    u32 chunk_idx = index / pool->object_count_per_chunk;
    u32 object_idx = index % pool->object_count_per_chunk;

    u32 magic_src = guid_get_magic(object);
    u32 magic_dst = pool->magic_chunks[chunk_idx][object_idx];

    check(magic_src == magic_dst, "[ %s ] guid invalid (bad magic) : %u != %u", pool->name, magic_src, magic_dst);

    pool->magic_chunks[chunk_idx][object_idx] = 0xFFFFFFFF;

    pool->object_count--;

    // _write_index_from_object_count()
    chunk_idx = pool->object_count / pool->object_count_per_chunk;
    object_idx = pool->object_count % pool->object_count_per_chunk;

    pool->index_chunks[chunk_idx][object_idx] = index;


    ////////////////////////////////////////
#ifdef CSR_TRACE_OBJECT_POOL
    clog_trace("<<< object_pool_free( %s ) : guid = %lu, magic = %u, index = %u [ chunk: %u, object: %u ]",
        pool->name, object.id, magic, index, chunk_idx, object_idx);

    size_t byte_size_used = pool->object_count * pool->object_size;
    size_t byte_size_total = pool->object_count_max * pool->object_size;

    clog_trace("  = slots used : %u/%u ( %lu/%lu bytes )",
        pool->object_count, pool->object_count_max, byte_size_used, byte_size_total);
#endif
    ////////////////////////////////////////

error:
    return;
}

void* object_pool_get(struct object_pool* pool, guid object)
{
    check_ptr(pool);

    u32 index = guid_get_value(object);
    check(index < pool->object_count_max, "[ %s ] guid invalid (bad index) : %u", pool->name, index);

    u32 chunk_idx = index / pool->object_count_per_chunk;
    u32 object_idx = index % pool->object_count_per_chunk;

    u32 magic_src = guid_get_magic(object);
    u32 magic_dst = pool->magic_chunks[chunk_idx][object_idx];

    check(magic_src == magic_dst, "[ %s ] guid invalid (bad magic) : %u != %u", pool->name, magic_src, magic_dst);

    return pool->object_chunks[chunk_idx] + (object_idx * pool->object_size);

error:
    return NULL;
}
