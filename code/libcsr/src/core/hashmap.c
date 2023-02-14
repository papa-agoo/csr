////////////////////////////////////////////////////////////////////////////////////////////////////

#include <bstrlib.h> // FIXME remove dep

#include <csr/core/vector.h>
#include <csr/core/hashmap.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define HASHMAP_NODE_COUNT 64
#define HASHMAP_BUCKET_COUNT 64

struct hashmap_node;

typedef u32 (*hashmap_hash_cb)(void *key);
typedef s32 (*hashmap_compare_cb)(void *a, void *b);

struct hashmap
{
    struct vector* buckets;

    hashmap_hash_cb hash_cb;
    hashmap_compare_cb compare_cb;
};

struct hashmap_node
{
    u32 hash;

    void *key;
    void *data;
};

////////////////////////////////////////

static s32 _default_compare_cb(void *a, void *b)
{
    return bstrcmp((bstring)a, (bstring)b);
}

static u32 _default_hash_cb(void *key)
{
    size_t len = blength((bstring)key);
    char *key_str = bdata((bstring)key);

    u32 hash = 0;

    for (u32 i = 0; i < len; ++i)
    {
        hash += key_str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

CSR_INLINE struct vector* _find_bucket(struct hashmap* map, void* key, u32* hash_out, bool create)
{
    u32 hash = map->hash_cb(key);

    u32 bucket_idx = hash % HASHMAP_BUCKET_COUNT;
    check_expr(bucket_idx >= 0);

    struct vector* bucket = *(struct vector**)vector_get(map->buckets, bucket_idx);

    if (!bucket && create)
    {
        bucket = vector_create(HASHMAP_NODE_COUNT, sizeof(struct hashmap_node));
        check_mem(bucket);

        vector_set(map->buckets, bucket_idx, bucket);
    }

    *hash_out = hash;

    return bucket;

error:
    return NULL;
}

CSR_INLINE s32 _find_node(struct hashmap* map, u32 hash, struct vector* bucket, void* key)
{
    for (u32 i = 0; i < vector_size(bucket); i++)
    {
        struct hashmap_node* node = vector_get(bucket, i);
        check_ptr(node);

        if (node->hash == hash && map->compare_cb(node->key, key) == 0) {
            return i;
        }
    }

error:
    return -1;
}

////////////////////////////////////////

struct hashmap* hashmap_create()
{
    struct hashmap* map = calloc(1, sizeof(struct hashmap));
    check_mem(map);

    map->compare_cb = _default_compare_cb;
    map->hash_cb = _default_hash_cb;

    map->buckets = vector_create(HASHMAP_BUCKET_COUNT, sizeof(struct vector*));
    check_mem(map->buckets);

    return map;

error:
    if (map) {
        hashmap_destroy(map);
    }

    return NULL;
}

void hashmap_destroy(struct hashmap* map)
{
    check_ptr(map);
    check_ptr(map->buckets);

    for (u32 i = 0; i < vector_size(map->buckets); i++)
    {
        struct vector* bucket = *(struct vector**)vector_get(map->buckets, i);

        if (bucket) {
            vector_destroy(bucket);
        }
    }

    vector_destroy(map->buckets);

    free(map);

error:
    return;
}

void* hashmap_set(struct hashmap *map, const char *key, void *data)
{
    u32 hash = 0;

    struct vector* bucket = _find_bucket(map, bfromcstr(key), &hash, true);
    check_quiet(bucket);

    ////////////////////////////////////////

    if (data)
    {
        struct hashmap_node node = {0};
        node.hash = hash;
        node.key = bfromcstr(key);
        node.data = data;

        vector_push_back(bucket, node);

        return NULL;
    }

    ////////////////////////////////////////

    s32 node_idx = _find_node(map, hash, bucket, bfromcstr(key));
    check_expr(node_idx >= 0);

    struct hashmap_node *node = vector_get(bucket, node_idx);
    check_ptr(node);

    void *data_old = node->data;

    vector_set_(bucket, node_idx, NULL);

    return data_old;

    ////////////////////////////////////////

error:
    return NULL;
}

void *hashmap_get(struct hashmap *map, const char *key)
{
    u32 hash = 0;

    struct vector* bucket = _find_bucket(map, bfromcstr(key), &hash, false);
    check_quiet(bucket);

    s32 node_idx = _find_node(map, hash, bucket, bfromcstr(key));
    check_expr(node_idx >= 0);

    struct hashmap_node *node = vector_get(bucket, node_idx);
    check_ptr(node);

    return node->data;

error:
    return NULL;
}

bool hashmap_has(struct hashmap* map, const char* key)
{
    return hashmap_get(map, key) != NULL;
}

s32 hashmap_traverse(struct hashmap* map, hashmap_traverse_cb traverse_cb, void *user_data)
{
    check_ptr(map);
    check_ptr(map->buckets);
    check_ptr(traverse_cb);

    for (u32 i = 0; i < vector_size(map->buckets); i++)
    {
        struct vector* bucket = *(struct vector**)vector_get(map->buckets, i);
        if (!bucket) continue;

        for (u32 j = 0; j < vector_size(bucket); j++)
        {
            struct hashmap_node *node = vector_get(bucket, j);
            if (!node) continue;

            s32 rc = traverse_cb(bdata((bstring)node->key), node->data, user_data);

            // user requested abort
            if (rc < 0 ) return rc;
        }
    }

error:
    return -1;
}

void* hashmap_find(struct hashmap* map, void *left, hashmap_find_cb find_cb)
{
    check_ptr(map);
    check_ptr(map->buckets);
    check_ptr(find_cb);

    for (u32 i = 0; i < vector_size(map->buckets); i++)
    {
        struct vector* bucket = *(struct vector**)vector_get(map->buckets, i);
        if (!bucket) continue;

        for (u32 j = 0; j < vector_size(bucket); j++)
        {
            struct hashmap_node *node = vector_get(bucket, j);
            if (!node) continue;

            void* data = find_cb(bdata((bstring)node->key), left, node->data);
            if (data) return data;
        }
    }

error:
    return NULL;
}
