////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct softgl_driver* softgl_driver_ptr()
{
    static struct softgl_driver driver = {0};

    return &driver;
}

static result_e _init_driver_storage(struct softgl_storage *storage)
{
    check_ptr(storage);

    storage->shaders = object_pool_create("SoftGL Shaders", sizeof(struct softgl_shader));
    check_ptr(storage->shaders);

    storage->pipelines = object_pool_create("SoftGL Pipelines", sizeof(struct softgl_pipeline));
    check_ptr(storage->pipelines);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _cleanup_driver_storage(struct softgl_storage *storage)
{
    check_ptr(storage);

    if (storage->shaders)
        object_pool_destroy(storage->shaders);

    if (storage->pipelines)
        object_pool_destroy(storage->pipelines);

error:
    return;
}

static result_e _init_driver_state(struct softgl_state *state)
{
    check_ptr(state);

    // ...

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e softgl_init_driver()
{
    struct softgl_driver *driver = softgl_driver_ptr();

    driver->info.name = make_string("SoftGL");
    driver->info.version = make_version(0, 0, 1);

    check_result(_init_driver_state(&driver->state), "could not init softgl driver state");
    check_result(_init_driver_storage(&driver->storage), "could not init softgl driver storage");

    driver->is_initialized = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void softgl_quit_driver()
{
    _cleanup_driver_storage(softgl_storage_ptr());

error:
    return;
}

void softgl_get_driver_info(struct softgl_driver_info *info)
{
    check_ptr(info);

    *info = softgl_driver_ptr()->info;

error:
    return;
}

