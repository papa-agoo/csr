////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct object_pool;

struct object_pool* object_pool_create(struct string name, size_t object_size);
void object_pool_destroy(struct object_pool *pool);

guid object_pool_alloc(struct object_pool *pool, void *object);
void object_pool_free(struct object_pool *pool, guid object);
void *object_pool_get(struct object_pool *pool, guid object);
