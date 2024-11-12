////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct xgl_driver* xgl_driver_ptr()
{
    static struct xgl_driver driver = {0};

    return &driver;
}

static void _cleanup_driver_storage(struct xgl_storage *storage)
{
    check_ptr(storage);

    if (storage->pipeline_layouts)
        object_pool_destroy(storage->pipeline_layouts);

    if (storage->descriptor_sets)
        object_pool_destroy(storage->descriptor_sets);

    if (storage->descriptor_set_layouts)
        object_pool_destroy(storage->descriptor_set_layouts);

    if (storage->shaders)
        object_pool_destroy(storage->shaders);

    if (storage->pipelines)
        object_pool_destroy(storage->pipelines);

    if (storage->buffers)
        object_pool_destroy(storage->buffers);

    if (storage->textures)
        object_pool_destroy(storage->textures);

    if (storage->samplers)
        object_pool_destroy(storage->samplers);

    if (storage->framebuffers)
        object_pool_destroy(storage->framebuffers);

    if (storage->swapchains)
        object_pool_destroy(storage->swapchains);

error:
    return;
}

static result_e _init_driver_storage(struct xgl_storage *storage)
{
    check_ptr(storage);

    storage->swapchains = object_pool_create(make_string("XGL Swapchains"), sizeof(struct xgl_swapchain));
    check_ptr(storage->swapchains);

    storage->framebuffers = object_pool_create(make_string("XGL Framebuffers"), sizeof(struct xgl_framebuffer));
    check_ptr(storage->framebuffers);

    storage->samplers = object_pool_create(make_string("XGL Samplers"), sizeof(struct xgl_sampler));
    check_ptr(storage->samplers);

    storage->textures = object_pool_create(make_string("XGL Textures"), sizeof(struct xgl_texture));
    check_ptr(storage->textures);

    storage->buffers = object_pool_create(make_string("XGL Buffers"), sizeof(struct xgl_buffer));
    check_ptr(storage->buffers);

    storage->pipelines = object_pool_create(make_string("XGL Pipelines"), sizeof(struct xgl_pipeline));
    check_ptr(storage->pipelines);

    storage->pipeline_layouts = object_pool_create(make_string("XGL Pipeline Layouts"), sizeof(struct xgl_pipeline_layout));
    check_ptr(storage->pipeline_layouts);

    storage->shaders = object_pool_create(make_string("XGL Shaders"), sizeof(struct xgl_shader));
    check_ptr(storage->shaders);

    storage->descriptor_set_layouts = object_pool_create(make_string("XGL Descriptor Set Layouts"), sizeof(struct xgl_descriptor_set_layout));
    check_ptr(storage->descriptor_set_layouts);

    storage->descriptor_sets = object_pool_create(make_string("XGL Descriptor Sets"), sizeof(struct xgl_descriptor_set));
    check_ptr(storage->descriptor_sets);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _init_driver_state(struct xgl_state *state)
{
    // FIXME AREA :)
    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e xgl_init_driver()
{
    struct xgl_driver *driver = xgl_driver_ptr();

    ////////////////////////////////////////

    check_result(xgl_init_driver_impl(&driver->info));

    check_result(_init_driver_storage(xgl_storage_ptr()));
    check_result(_init_driver_state(xgl_state_ptr()));

    ////////////////////////////////////////

    driver->is_initialized = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_quit_driver()
{
    xgl_quit_driver_impl();

    // _cleanup_driver_state();
    _cleanup_driver_storage(xgl_storage_ptr());

error:
    return;
}

void xgl_get_driver_info(struct xgl_driver_info *info)
{
    check_ptr(info);

    *info = xgl_driver_ptr()->info;

error:
    return;
}
