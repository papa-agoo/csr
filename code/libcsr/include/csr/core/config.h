////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// workflows
//  - a) modifying internal storage directly (get / set calls)
//  - b) referencing external storage (map calls)

// misc notes
//  - internal storage : actual data which is used for file i/o (read / write ini)
//  - external storage : key / value mappings to external storage (get synced with internal storage)

// FIXME
//  - preserve section / key order on ini export

////////////////////////////////////////////////////////////////////////////////////////////////////

struct config;

struct config* config_create_from_ini(const char *filename);

struct config* config_create();
void config_destroy(struct config *cfg);

void config_clear(struct config *cfg);
void config_flush(struct config *cfg);
void config_dump(struct config *cfg);

result_e config_load_ini(struct config *cfg, const char *filename);
result_e config_save_ini(struct config *cfg, const char *filename);

const char* config_get_filename(struct config *cfg);

bool config_get_int(struct config *cfg, const char *key, s32 *out);
bool config_set_int(struct config *cfg, const char *key, s32 value);
bool config_map_int(struct config *cfg, const char *key, s32 *value);

bool config_get_float(struct config *cfg, const char *key, f32 *out);
bool config_set_float(struct config *cfg, const char *key, f32 value);
bool config_map_float(struct config *cfg, const char *key, f32 *value);

bool config_get_bool(struct config *cfg, const char *key, bool *out);
bool config_set_bool(struct config *cfg, const char *key, bool value);
bool config_map_bool(struct config *cfg, const char *key, bool *value);

bool config_get_str(struct config *cfg, const char *key, const char **out);
bool config_set_str(struct config *cfg, const char *key, const char *value);
bool config_map_str(struct config *cfg, const char *key, const char **value);

bool config_get_vec2(struct config *cfg, const char *key, struct vec2 *out);
bool config_set_vec2(struct config *cfg, const char *key, struct vec2 value);
bool config_map_vec2(struct config *cfg, const char *key, struct vec2 *value);

bool config_get_vec3(struct config *cfg, const char *key, struct vec3 *out);
bool config_set_vec3(struct config *cfg, const char *key, struct vec3 value);
bool config_map_vec3(struct config *cfg, const char *key, struct vec3 *value);

bool config_get_vec4(struct config *cfg, const char *key, struct vec4 *out);
bool config_set_vec4(struct config *cfg, const char *key, struct vec4 value);
bool config_map_vec4(struct config *cfg, const char *key, struct vec4 *value);
