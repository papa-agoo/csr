////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// path validation: use regex
// struct path *path = path_create("{FOO_BAR}/baz", resolver_cb);
// struct path *path = path_create(resolver_cb, "{FOO_BAR}/%s", "baz");

// path_set(path, "{FOO_BAR}/baz");

// const char *path_str = path_get(path);       // {FOO_BAR}/baz
// const char *path_str = path_get_real(path);  // /tmp/csr/baz

result_e path_create_vars();
void path_destroy_vars();

typedef void (*path_traverse_vars_cb_t)(const char *key);
void path_traverse_vars(path_traverse_vars_cb_t callback);

const char* path_get_var(const char *var);
result_e path_set_var(const char *var, const char *value);

const char* path_get(const char *path);
// const char* path_get(const char *path, ...); // FIXME makes path_merge() obsolete
const char* path_merge(const char *path, const char *element);

const char* path_get_dirname(const char *path); // FIXME
const char* path_get_basename(const char *path); // FIXME
const char* path_get_extension(const char *path); // FIXME
