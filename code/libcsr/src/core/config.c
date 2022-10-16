////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/config.h>
#include <csr/core/hashmap.h>
#include <csr/core/file_io.h>

#include <iniparser.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define STR_BUF_SIZE 64
static char g_str_buf[STR_BUF_SIZE];

struct config
{
    const char *filename;

    // internal storage
    dictionary *ini;

    // external storage (pointers)
    struct {
        struct hashmap *ints;
        struct hashmap *floats;
        struct hashmap *bools;
        struct hashmap *strings;
        struct hashmap *vec2s;
        struct hashmap *vec3s;
        struct hashmap *vec4s;
    } map;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// internal helpers
////////////////////////////////////////////////////////////////////////////////////////////////////
static s32 _iniparser_error_callback(const char *format, ...) { return 0; }

CSR_INLINE bool _config_has_entries(struct config *cfg)
{
    return iniparser_getnsec(cfg->ini) > 0;
}

CSR_INLINE bool _config_key_valid(struct config *cfg, const char *key)
{
    // FIXME
    return true;
}

CSR_INLINE const char* _get_section_from_key(const char *key)
{
    char *str = strdup(key);

    return strsep(&str, ":");
}

CSR_INLINE bool _ini_set_value(dictionary *ini, const char *key, const char *value)
{
    const char *section = _get_section_from_key(key);
    check(section, "failed to extract section");

    iniparser_set(ini, section, NULL);
    iniparser_set(ini, key, value);

    return true;

error:
    return false;
}

CSR_INLINE s32 _sync_int_ini_to_ref(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    s32 *value = data;

    config_get_int(cfg, key, value);

    clog_trace("%s << %d", key, *value);

    return 0;
}

CSR_INLINE s32 _sync_int_ref_to_ini(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    s32 *value = data;

    config_set_int(cfg, key, *value);

    clog_trace("%s >> %d", key, *value);

    return 0;
}

CSR_INLINE s32 _sync_float_ini_to_ref(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    f32 *value = data;

    config_get_float(cfg, key, value);

    clog_trace("%s << %.2f", key, *value);

    return 0;
}

CSR_INLINE s32 _sync_float_ref_to_ini(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    f32 *value = data;

    config_set_float(cfg, key, *value);

    clog_trace("%s >> %.2f", key, *value);

    return 0;
}

CSR_INLINE s32 _sync_bool_ini_to_ref(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    bool *value = data;

    config_get_bool(cfg, key, value);

    clog_trace("%s << %d", key, *value);

    return 0;
}

CSR_INLINE s32 _sync_bool_ref_to_ini(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    bool *value = data;

    config_set_bool(cfg, key, *value);

    clog_trace("%s >> %d", key, *value);

    return 0;
}

CSR_INLINE s32 _sync_str_ini_to_ref(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    const char **value = data;

    config_get_str(cfg, key, value);

    clog_trace("%s << %s", key, *value);

    return 0;
}

CSR_INLINE s32 _sync_str_ref_to_ini(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    const char **value = data;

    config_set_str(cfg, key, *value);

    clog_trace("%s >> %s", key, *value);

    return 0;
}

CSR_INLINE s32 _sync_vec2_ini_to_ref(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    struct vec2* value = data;

    config_get_vec2(cfg, key, value);

    clog_trace("%s << (%.2f, %.2f)", key, value->x, value->y);

    return 0;
}

CSR_INLINE s32 _sync_vec2_ref_to_ini(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    struct vec2* value = data;

    config_set_vec2(cfg, key, *value);

    clog_trace("%s >> (%.2f, %.2f)", key, value->x, value->y);

    return 0;
}

CSR_INLINE s32 _sync_vec3_ini_to_ref(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    struct vec3* value = data;

    config_get_vec3(cfg, key, value);

    clog_trace("%s << (%.2f, %.2f, %.2f)", key, value->x, value->y, value->z);

    return 0;
}

CSR_INLINE s32 _sync_vec3_ref_to_ini(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    struct vec3* value = data;

    config_set_vec3(cfg, key, *value);

    clog_trace("%s >> (%.2f, %.2f, %.2f)", key, value->x, value->y, value->z);

    return 0;
}

CSR_INLINE s32 _sync_vec4_ini_to_ref(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    struct vec4* value = data;

    config_get_vec4(cfg, key, value);

    clog_trace("%s << (%.2f, %.2f, %.2f, %.2f)", key, value->x, value->y, value->z, value->w);

    return 0;
}

CSR_INLINE s32 _sync_vec4_ref_to_ini(const char* key, void* data, void* user_data)
{
    struct config *cfg = user_data;
    struct vec4* value = data;

    config_set_vec4(cfg, key, *value);

    clog_trace("%s >> (%.2f, %.2f, %.2f, %.2f)", key, value->x, value->y, value->z, value->w);

    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// public api
////////////////////////////////////////////////////////////////////////////////////////////////////
struct config* config_create_from_ini(const char *filename)
{
    check_ptr(filename);

    struct config *cfg = config_create();
    check_ptr(cfg);

    config_load_ini(cfg, filename);

    return cfg;

error:
    return NULL;
}

struct config* config_create()
{
    struct config *cfg = calloc(1, sizeof(struct config));
    check_mem(cfg);

    cfg->filename = NULL;

    cfg->ini = dictionary_new(0);
    check_mem(cfg->ini);

    iniparser_set_error_callback(_iniparser_error_callback);

    ////////////////////////////////////////

    cfg->map.ints = hashmap_create();
    check_ptr(cfg->map.ints);

    cfg->map.floats = hashmap_create();
    check_ptr(cfg->map.floats);

    cfg->map.bools = hashmap_create();
    check_ptr(cfg->map.bools);

    cfg->map.strings = hashmap_create();
    check_ptr(cfg->map.strings);

    cfg->map.vec2s = hashmap_create();
    check_ptr(cfg->map.vec2s);

    cfg->map.vec3s = hashmap_create();
    check_ptr(cfg->map.vec3s);

    cfg->map.vec4s = hashmap_create();
    check_ptr(cfg->map.vec4s);

    ////////////////////////////////////////

    return cfg;

error:
    return NULL;
}

void config_destroy(struct config *cfg)
{
    check_ptr(cfg);

    hashmap_destroy(cfg->map.ints);
    hashmap_destroy(cfg->map.floats);
    hashmap_destroy(cfg->map.bools);
    hashmap_destroy(cfg->map.strings);
    hashmap_destroy(cfg->map.vec2s);
    hashmap_destroy(cfg->map.vec3s);
    hashmap_destroy(cfg->map.vec4s);

    iniparser_freedict(cfg->ini);

error:
    return;
}

void config_clear(struct config *cfg)
{
    check_ptr(cfg);

    if (cfg->ini) {
        iniparser_freedict(cfg->ini);
    }

    cfg->ini = dictionary_new(0);
    check_ptr(cfg->ini);
    
error:
    return;
}

void config_flush(struct config *cfg)
{
    check_ptr(cfg);
    check(cfg->filename, "ini filename not set, cannot flush config");

    if (_config_has_entries(cfg)) {
        config_save_ini(cfg, cfg->filename);
    }

error:
    return;
}

void config_dump(struct config *cfg)
{
    check_ptr(cfg);

    const char *filename = (cfg->filename) ? (cfg->filename) : "???";

    clog_info(">>> dump : %s", filename);

    iniparser_dump_ini(cfg->ini, stdout);

    clog_info("<<< dump : %s", filename);

error:
    return;
}

result_e config_load_ini(struct config *cfg, const char *filename)
{
    check_ptr(cfg);
    check_ptr(filename);

    clog_trace("loading config from ini : %s", filename);

    ////////////////////////////////////////

    cfg->filename = strdup(filename);

    dictionary *ini = iniparser_load(filename);
    check_quiet(ini);

    if (cfg->ini) {
        iniparser_freedict(cfg->ini);
    }

    cfg->ini = ini;

    ////////////////////////////////////////

    // copy values from internal storage (ini) to external storage (refs)
    hashmap_traverse(cfg->map.ints, _sync_int_ini_to_ref, cfg);
    hashmap_traverse(cfg->map.floats, _sync_float_ini_to_ref, cfg);
    hashmap_traverse(cfg->map.bools, _sync_bool_ini_to_ref, cfg);
    hashmap_traverse(cfg->map.strings, _sync_str_ini_to_ref, cfg);
    hashmap_traverse(cfg->map.vec2s, _sync_vec2_ini_to_ref, cfg);
    hashmap_traverse(cfg->map.vec3s, _sync_vec3_ini_to_ref, cfg);
    hashmap_traverse(cfg->map.vec4s, _sync_vec4_ini_to_ref, cfg);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e config_save_ini(struct config *cfg, const char *filename)
{
    check_ptr(cfg);
    check_ptr(filename);

    clog_trace("saving config to ini : %s", filename);

    check_expr(_config_has_entries(cfg));

    ////////////////////////////////////////

    // copy values from external storage (refs) to internal storage (ini)
    hashmap_traverse(cfg->map.ints, _sync_int_ref_to_ini, cfg);
    hashmap_traverse(cfg->map.floats, _sync_float_ref_to_ini, cfg);
    hashmap_traverse(cfg->map.bools, _sync_bool_ref_to_ini, cfg);
    hashmap_traverse(cfg->map.strings, _sync_str_ref_to_ini, cfg);
    hashmap_traverse(cfg->map.vec2s, _sync_vec2_ref_to_ini, cfg);
    hashmap_traverse(cfg->map.vec3s, _sync_vec3_ref_to_ini, cfg);
    hashmap_traverse(cfg->map.vec4s, _sync_vec4_ref_to_ini, cfg);

    ////////////////////////////////////////

    fio_file *file = fio_open(filename, FIO_MODE_WRITE_ONLY);
    check(file, "could not open file for writing : %s", filename);

    iniparser_dump_ini(cfg->ini, fio_get_fp(file));

    fio_close(file);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

const char* config_get_filename(struct config *cfg)
{
    check_ptr(cfg);

    return cfg->filename;

error:
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool config_get_int(struct config *cfg, const char *key, s32 *out)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(out);

    *out = (s32) iniparser_getint(cfg->ini, key, *out);

    return true;

error:
    return false;
}

bool config_set_int(struct config *cfg, const char *key, s32 value)
{
    check_ptr(cfg);
    check_ptr(key);

    snprintf(g_str_buf, STR_BUF_SIZE, "%d", value);

    return _ini_set_value(cfg->ini, key, strdup(g_str_buf));

error:
    return false;
}

bool config_map_int(struct config *cfg, const char *key, s32* value)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(value);

    config_get_int(cfg, key, value);
    config_set_int(cfg, key, *value);

    hashmap_set(cfg->map.ints, key, value);

    clog_trace("%s << %d", key, *value);

    return true;

error:
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool config_get_float(struct config *cfg, const char *key, f32 *out)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(out);

    *out = (f32) iniparser_getdouble(cfg->ini, key, *out);

    return true;

error:
    return false;
}

bool config_set_float(struct config *cfg, const char *key, f32 value)
{
    check_ptr(cfg);
    check_ptr(key);

    snprintf(g_str_buf, STR_BUF_SIZE, "%.2f", value);

    return _ini_set_value(cfg->ini, key, strdup(g_str_buf));

error:
    return false;
}

bool config_map_float(struct config *cfg, const char *key, f32* value)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(value);

    config_get_float(cfg, key, value);
    config_set_float(cfg, key, *value);

    hashmap_set(cfg->map.floats, key, value);

    clog_trace("%s << %.2f", key, *value);

    return true;

error:
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool config_get_bool(struct config *cfg, const char *key, bool *out)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(out);

    *out = (bool) iniparser_getboolean(cfg->ini, key, *out);

error:
    return false;
}

bool config_set_bool(struct config *cfg, const char *key, bool value)
{
    check_ptr(cfg);
    check_ptr(key);

    snprintf(g_str_buf, STR_BUF_SIZE, "%d", value);

    return _ini_set_value(cfg->ini, key, strdup(g_str_buf));

error:
    return false;
}

bool config_map_bool(struct config *cfg, const char *key, bool* value)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(value);

    config_get_bool(cfg, key, value);
    config_set_bool(cfg, key, *value);

    hashmap_set(cfg->map.bools, key, value);

    clog_trace("%s << %d", key, *value);

    return true;

error:
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool config_get_str(struct config *cfg, const char *key, const char **out)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(out);

    *out = strdup( iniparser_getstring(cfg->ini, key, *out) );

    return true;

error:
    return false;   
}

bool config_set_str(struct config *cfg, const char *key, const char *value)
{
    check_ptr(cfg);
    check_ptr(key);

    return _ini_set_value(cfg->ini, key, value);

error:
    return false;   
}

bool config_map_str(struct config *cfg, const char *key, const char** value)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(value);

    config_get_str(cfg, key, value);
    config_set_str(cfg, key, *value);

    hashmap_set(cfg->map.strings, key, value);

    clog_trace("%s << %s", key, *value);

    return true;

error:
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool config_get_vec2(struct config *cfg, const char *key, struct vec2 *out)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(out);

    const char *str_ro = iniparser_getstring(cfg->ini, key, NULL);

    if (str_ro)
    {
        char *str = strdup(str_ro);

        const char *x_str = strsep(&str, ",");
        check_ptr(x_str);

        const char *y_str = strsep(&str, ",");
        check_ptr(y_str);

        out->x = (f32) atof(x_str);
        out->y = (f32) atof(y_str);
    }

    return true;

error:
    return false;
}

bool config_set_vec2(struct config *cfg, const char *key, struct vec2 value)
{
    check_ptr(cfg);
    check_ptr(key);

    snprintf(g_str_buf, STR_BUF_SIZE, "%.2f, %.2f", value.x, value.y);

    return _ini_set_value(cfg->ini, key, strdup(g_str_buf));

error:
    return false;   
}

bool config_map_vec2(struct config *cfg, const char *key, struct vec2* value)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(value);

    config_get_vec2(cfg, key, value);
    config_set_vec2(cfg, key, *value);

    hashmap_set(cfg->map.vec2s, key, value);

    clog_trace("%s << (%.2f, %.2f)", key, value->x, value->y);

    return true;

error:
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool config_get_vec3(struct config *cfg, const char *key, struct vec3 *out)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(out);

    const char *str_ro = iniparser_getstring(cfg->ini, key, NULL);

    if (str_ro)
    {
        char *str = strdup(str_ro);

        const char *x_str = strsep(&str, ",");
        check_ptr(x_str);

        const char *y_str = strsep(&str, ",");
        check_ptr(y_str);

        const char *z_str = strsep(&str, ",");
        check_ptr(z_str);

        out->x = (f32) atof(x_str);
        out->y = (f32) atof(y_str);
        out->z = (f32) atof(z_str);
    }

    return true;

error:
    return false;
}

bool config_set_vec3(struct config *cfg, const char *key, struct vec3 value)
{
    check_ptr(cfg);
    check_ptr(key);

    snprintf(g_str_buf, STR_BUF_SIZE, "%.2f, %.2f, %.2f", value.x, value.y, value.z);

    return _ini_set_value(cfg->ini, key, strdup(g_str_buf));

error:
    return false;   
}

bool config_map_vec3(struct config *cfg, const char *key, struct vec3* value)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(value);

    config_get_vec3(cfg, key, value);
    config_set_vec3(cfg, key, *value);

    hashmap_set(cfg->map.vec3s, key, value);

    clog_trace("%s << (%.2f, %.2f, %.2f)", key, value->x, value->y, value->z);

    return true;

error:
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool config_get_vec4(struct config *cfg, const char *key, struct vec4 *out)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(out);

    const char *str_ro = iniparser_getstring(cfg->ini, key, NULL);

    if (str_ro)
    {
        char *str = strdup(str_ro);

        const char *x_str = strsep(&str, ",");
        check_ptr(x_str);

        const char *y_str = strsep(&str, ",");
        check_ptr(y_str);

        const char *z_str = strsep(&str, ",");
        check_ptr(z_str);

        const char *w_str = strsep(&str, ",");
        check_ptr(w_str);

        out->x = (f32) atof(x_str);
        out->y = (f32) atof(y_str);
        out->z = (f32) atof(z_str);
        out->w = (f32) atof(w_str);
    }

    return true;

error:
    return false;
}

bool config_set_vec4(struct config *cfg, const char *key, struct vec4 value)
{
    check_ptr(cfg);
    check_ptr(key);

    snprintf(g_str_buf, STR_BUF_SIZE, "%.2f, %.2f, %.2f, %.2f", value.x, value.y, value.z, value.w);

    return _ini_set_value(cfg->ini, key, strdup(g_str_buf));

error:
    return false;
}

bool config_map_vec4(struct config *cfg, const char *key, struct vec4* value)
{
    check_ptr(cfg);
    check_ptr(key);
    check_ptr(value);

    config_get_vec4(cfg, key, value);
    config_set_vec4(cfg, key, *value);

    hashmap_set(cfg->map.vec4s, key, value);

    clog_trace("%s << (%.2f, %.2f, %.2f, %.2f)", key, value->x, value->y, value->z, value->w);

    return true;

error:
    return false;
}
