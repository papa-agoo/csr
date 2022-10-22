////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct gl_driver* gl_driver_ptr()
{
    static struct xgl_driver_info info = {
        .name = "OpenGL",
        .type = XGL_DRIVER_TYPE_OPENGL,
    };

    static struct gl_driver driver = {0};

    return &driver;
}

static void _cleanup_driver_storage(struct gl_storage *storage)
{
    check_ptr(storage);

    if (storage->framebuffers)
        object_pool_destroy(storage->framebuffers);

    if (storage->samplers)
        object_pool_destroy(storage->samplers);

    if (storage->textures)
        object_pool_destroy(storage->textures);

    if (storage->buffers)
        object_pool_destroy(storage->buffers);

    if (storage->shaders)
        object_pool_destroy(storage->shaders);

    if (storage->pipelines)
        object_pool_destroy(storage->pipelines);

error:
    return;
}

static result_e _init_driver_storage(struct gl_storage *storage)
{
    check_ptr(storage);

    storage->framebuffers = object_pool_create("OpenGL Framebuffers", sizeof(struct gl_framebuffer));
    check_ptr(storage->framebuffers)
    
    storage->samplers = object_pool_create("OpenGL Samplers", sizeof(struct gl_sampler));
    check_ptr(storage->samplers)
    
    storage->textures = object_pool_create("OpenGL Texures", sizeof(struct gl_texture));
    check_ptr(storage->textures)
    
    storage->buffers = object_pool_create("OpenGL Buffers", sizeof(struct gl_buffer));
    check_ptr(storage->buffers)
    
    storage->shaders = object_pool_create("OpenGL Shaders", sizeof(struct gl_shader));
    check_ptr(storage->shaders)
    
    storage->pipelines = object_pool_create("OpenGL Pipelines", sizeof(struct gl_pipeline));
    check_ptr(storage->pipelines)

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _init_driver_info(struct xgl_driver_info *info)
{
    info->name = "OpenGL";
    info->type = XGL_DRIVER_TYPE_OPENGL;

    // DSA needs opengl v4.5+
    info->version = make_version(4, 5, 0);
}

result_e xgl_init_driver_impl(struct xgl_driver_info *info)
{
    csr_assert(!gl_driver_ptr()->is_initialized);

    _init_driver_info(info);

    opengl_init_debug_callbacks();

    ////////////////////////////////////////

    result_e result = _init_driver_storage(gl_storage_ptr());
    check_result(result, "could not init opengl driver storage");

    gl_driver_ptr()->is_initialized = true;

    return RC_SUCCESS;

error:

    _cleanup_driver_storage(gl_storage_ptr());

    return RC_FAILURE;
}

void xgl_quit_driver_impl()
{
    csr_assert(gl_driver_ptr()->is_initialized);

    _cleanup_driver_storage(gl_storage_ptr());

    gl_driver_ptr()->is_initialized = false;
}
