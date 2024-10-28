////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/config.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define PATH_INI_FILE "/tmp/hello_config.ini"

////////////////////////////////////////////////////////////

static void internal_storage_new_config()
{
    // some data
    f32 ratio = 0.5;

    // create empty config
    struct config *config = config_create();
    check_ptr(config);

    ////////////////////////////////////////

    // get value (value not found, ratio stays at 0.5)
    config_get_float(config, "my_conf:ratio", &ratio);

    // set value
    config_set_float(config, "my_conf:ratio", 0.75);

    // get value (value found, ratio is now 0.75)
    config_get_float(config, "my_conf:ratio", &ratio);

    ////////////////////////////////////////

    // save config
    config_save_ini(config, make_string(PATH_INI_FILE));

    // print and destroy config
    config_dump(config);
    config_destroy(config);

error:
    return;
}

static void internal_storage_existing_config()
{
    // some data
    s32 old = 0;
    s32 new = 7;

    // load ini file
    struct config *config = config_create_from_ini(make_string(PATH_INI_FILE));
    check_ptr(config);

    // get value from config
    config_get_int(config, "my_conf:count", &old);

    // set new value
    config_set_int(config, "my_conf:count", new);

    // save config
    config_flush(config);

    // print and destroy config
    config_dump(config);
    config_destroy(config);

error:
    return;
}

////////////////////////////////////////////////////////////

static void external_storage_new_config()
{
    // some data
    s32 count = 1;
    f32 ratio = 0.5;

    // create empty config
    struct config *config = config_create();
    check_ptr(config);

    // map values (no ini loaded, data stays unchanged)
    config_map_int(config, "my_conf:count", &count);
    config_map_float(config, "my_conf:ratio", &ratio);

    // override mapped / referenced data using values from ini (if existing)
    config_load_ini(config, make_string(PATH_INI_FILE));

    // change data manually
    count = 2;

    // save config (mapped values are synced automatically when saving)
    config_save_ini(config, make_string(PATH_INI_FILE));

    // print and destroy config
    config_dump(config);
    config_destroy(config);

error:
    return;
}

////////////////////////////////////////////////////////////

struct my_conf
{
    const char *name;

    f32 factor;
    s32 count;

    bool enabled;

    struct vec2 size;
    struct vec3 position;
    struct vec4 color;
};

static void my_conf_set_defaults(struct my_conf *data)
{
    check_ptr(data);

    data->name = "<unknown>";

    data->factor = 1.0f;
    data->count = 100;

    data->enabled = true;

    data->size = make_vec2(300, 200);
    data->position = make_vec3(2, 3, 4);
    data->color = make_vec4(0.34, 0.21, 0.24, 1.0);

error:
    return;
}

static void external_storage_existing_config()
{
    // init struct defaults
    struct my_conf data = {0};
    my_conf_set_defaults(&data);

    // load ini file
    struct config *config = config_create_from_ini(make_string(PATH_INI_FILE));
    check_ptr(config);

    // map values (automatically writes loaded data to referenced values)
    config_map_str(config, "general/application:name", &data.name);
    config_map_int(config, "kernel.core/log:max_messages", &data.count);
    config_map_float(config, "kernel.audio/xal:volume", &data.factor);
    config_map_bool(config, "kernel.video/window:vsync", &data.enabled);
    config_map_vec2(config, "kernel.video/window:resolution", &data.size);
    config_map_vec3(config, "foo.bar/baz:position", &data.position);
    config_map_vec4(config, "foo.bar/baz:color", &data.color);

    // change struct values
    data.name = strdup("popo");
    data.count = 200;
    data.size = make_vec2(2560, 1440);

    // save ini file (automatically writes referenced values to ini)
    config_flush(config);

    // print and destroy config
    config_dump(config);
    config_destroy(config);

error:
    return;
}

result_e main()
{
    // workflow a) get / set calls (internal storage)
    internal_storage_new_config();
    internal_storage_existing_config();

    // workflow b) map calls (external storage)
    external_storage_new_config();
    external_storage_existing_config();

    return RC_SUCCESS;
}
