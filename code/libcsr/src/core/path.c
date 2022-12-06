////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/path.h>
#include <csr/core/hashmap.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME use regex for pattern checks
//      - https://stackoverflow.com/questions/8044081/how-to-do-regex-string-replacements-in-pure-c

////////////////////////////////////////////////////////////////////////////////////////////////////

#define STR_BUF_SIZE 256

struct path
{
    bool is_initialized;

    struct hashmap *vars;

    char str_buf[STR_BUF_SIZE];
};

static struct path g_path = {0};

#define check_path_vars() \
    check(g_path.is_initialized, "path vars not created yet?")

////////////////////////////////////////////////////////////

result_e path_create_vars()
{
    check(!g_path.is_initialized, "path vars already created!");

    g_path.vars = hashmap_create();
    check_ptr(g_path.vars);

    g_path.is_initialized = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void path_destroy_vars()
{
    check_path_vars();

    check_ptr(g_path.vars);
    hashmap_destroy(g_path.vars);

    g_path.is_initialized = false;

error:
    return;
}

static s32 _traverse_path_vars(const char* key, void* data, void* user_data)
{
    path_traverse_vars_cb_t callback = user_data;

    callback(key);
}

void path_traverse_vars(path_traverse_vars_cb_t callback)
{
    check_path_vars();

    check_ptr(callback);

    hashmap_traverse(g_path.vars, _traverse_path_vars, callback);

error:
    return;
}

const char* path_get_var(const char *var)
{
    check_path_vars();

    check_ptr(var);

    return hashmap_get(g_path.vars, var);

error:
    return NULL;
}

result_e path_set_var(const char *var, const char *value)
{
    check_path_vars();

    check_ptr(var);
    check_ptr(value);

    if (hashmap_get(g_path.vars, var)) {
        clog_warn("overriding path var: {%s} => %s", var, value);
    }

    hashmap_set(g_path.vars, var, strdup(value));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static bool _expand_path_var(const char* const path, const char **path_out)
{
    char *str_right = strdup(path);

    // var begins
    char* str_left = strsep(&str_right, "{");
    check_quiet(str_right);

    // var must end with }
    char *var = strsep(&str_right, "}");
    check(str_right, "missing closing } for var");

    // var seems to be valid, lookup value
    const char *value = hashmap_get(g_path.vars, var);
    check(value, "could not resolve var : %s", var);

    // var lookup successfull, merge value and surrounding (left / right) strings
    snprintf(g_path.str_buf, STR_BUF_SIZE, "%s%s%s", str_left, value, str_right);

    // output expaneded path
    *path_out = strdup(g_path.str_buf);

    return true;

error:
    return false;
}

const char* path_get(const char *path)
{
    check_path_vars();

    check_ptr(path);

    const char *path_real = path;

    while (_expand_path_var(path_real, &path_real));

    return path_real;

error:
    return NULL;
}

const char* path_merge(const char *path, const char *element)
{
    check_path_vars();

    check_ptr(path);
    check_ptr(element);

    snprintf(g_path.str_buf, STR_BUF_SIZE, "%s/%s", path_get(path), element);

    return strdup(g_path.str_buf);

error:
    return NULL;
}

const char* path_get_dirname(const char *path)
{
    check_path_vars();

    check_ptr(path);

    // FIXME
    return strdup(path);

error:
    return NULL;
}

const char* path_get_basename(const char *path)
{
    check_path_vars();

    check_ptr(path);

    char *str = strdup(path);
    char* token = NULL;

    // FIXME
    while (str) { token = strsep(&str, "/"); };

    return strdup(token);

error:
    return NULL;
}

const char* path_get_extension(const char *path)
{
    check_path_vars();

    check_ptr(path);

    // FIXME
    return strdup(path);

error:
    return NULL;
}

