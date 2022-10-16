////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct hashmap;

typedef s32 (*hashmap_traverse_cb)(const char* key, void* data, void* user_data);
typedef void* (*hashmap_find_cb)(const char* key, void* left, void* data);

struct hashmap* hashmap_create();
void hashmap_destroy(struct hashmap* map);

void* hashmap_set(struct hashmap* map, const char* key, void* data);
void* hashmap_get(struct hashmap* map, const char* key);

s32 hashmap_traverse(struct hashmap* map, hashmap_traverse_cb cb, void *user_data);
void* hashmap_find(struct hashmap* map, void* left, hashmap_find_cb cb);
