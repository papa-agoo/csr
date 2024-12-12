////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet/aio.h> // FIXME remove screen dep., use render targets

#include "rsx_priv.h"
#include "rsx/rgpu_priv.h"
#include "rsx/rcpu_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_render_data();
static void _destroy_render_data();

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct rsx g_rsx = {0};

struct rsx* rsx_ptr()
{
    return &g_rsx;
}

result_e rsx_init(struct rsx_init_info *info)
{
    check_ptr(info);
    check_ptr(info->conf);
    check_ptr(info->screen_rgpu);
    check_ptr(info->screen_rcpu);

    ////////////////////////////////////////

    struct rsx *rsx = rsx_ptr();

    rsx->conf = info->conf;

    // create gpu renderer
    rsx->screen.rgpu = info->screen_rgpu;
    {
        rsx->rgpu = rgpu_create();
        check_ptr(rsx->rgpu);
    }

    // create cpu renderer
    rsx->screen.rcpu = info->screen_rcpu;
    {
        rsx->rcpu = rcpu_create();
        check_ptr(rsx->rcpu);
    }

    ////////////////////////////////////////

    check_result(_create_render_data());

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_quit()
{
    _destroy_render_data();

    rcpu_destroy();
    rgpu_destroy();

error:
    return;
}

void rsx_tick()
{
    struct rsx *rsx = rsx_ptr();

    // build render structures (suitable for gpu/cpu renderers)
    {
        // ...
    }

    // copy frame data to gpu
    struct rsx_uniform_buffer_frame *frame_data = &rsx_get_render_data()->frame.data;
    {
        void *cpu = &frame_data->cpu;
        void *gpu = xgl_map_buffer(frame_data->gpu);

        memcpy(gpu, cpu, sizeof(frame_data->cpu));
        xgl_unmap_buffer(frame_data->gpu);
    }

    // update cpu screen aspect ratio
    screen_set_aspect_ratio(rsx->screen.rcpu, screen_get_aspect_ratio(rsx->screen.rgpu));

    // tick cpu renderer
    if (screen_begin(rsx->screen.rcpu, SCREEN_SURFACE_TYPE_CPU))
    {
        struct pixelbuffer *pb = screen_get_pixelbuffer(rsx->screen.rcpu);
        struct xgl_viewport vp = screen_get_viewport(rsx->screen.rcpu);

        rcpu_tick(pb, *(struct softgl_viewport*) &vp);

        screen_end();
    }

    // tick gpu renderer
    if (screen_begin(rsx->screen.rgpu, SCREEN_SURFACE_TYPE_GPU))
    {
        rgpu_tick(screen_get_viewport(rsx->screen.rgpu));

        screen_end();
    }

error:
    return;
}

const struct rsx_conf* rsx_get_conf()
{
    return rsx_ptr()->conf;
}

struct rsx_render_data* rsx_get_render_data()
{
    return &rsx_ptr()->render_data;
}

static result_e _create_shader_resource_frame(struct rsx_shader_resource_frame *resource)
{
    check_ptr(resource);

    // uniform buffer
    struct rsx_uniform_buffer_frame *ubo = &resource->data;
    {
        shader_data_frame_init(&ubo->cpu);

        struct xgl_buffer_create_info info = {0};
        info.usage_flags = XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        info.byte_length = sizeof(ubo->cpu);
        info.data = &ubo->cpu;

        check_result(xgl_create_buffer(&info, &ubo->gpu));
    }

    // resource binding
    struct rsx_shader_resource_binding *binding = &resource->binding;
    {
        // copy layout info from cache
        struct rgpu_cache *cache = rsx_rgpu_cache_ptr();

        binding->ds_layout = cache->ds_layout.frame;
        binding->pipeline_layout = cache->pipeline_layout.main;

        check_result(xgl_create_descriptor_set(binding->ds_layout, &binding->ds));

        // update descriptor set
        struct xgl_buffer_descriptor descriptor = {0};
        descriptor.binding = 0;
        descriptor.buffer = ubo->gpu;

        struct xgl_descriptor_set_update_info update_info = {0};
        update_info.buffer_descriptors = &descriptor;
        update_info.buffer_descriptor_count = 1;

        check_result(xgl_update_descriptor_set(binding->ds, &update_info));
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _destroy_shader_resource_frame(struct rsx_shader_resource_frame *resource)
{
    check_ptr(resource);

    xgl_destroy_buffer(resource->data.gpu);
    xgl_destroy_descriptor_set(resource->binding.ds);

error:
    return;
}

static result_e _create_render_data()
{
    struct rsx_render_data *render_data = rsx_get_render_data();

    // create global shader data
    check_result(_create_shader_resource_frame(&render_data->frame));

    // create pass data
    check_result(rsx_pass_meshes_create(rsx_pass_data_meshes_ptr()));
    check_result(rsx_pass_gizmos_create(rsx_pass_data_gizmos_ptr()));
    check_result(rsx_pass_environment_create(rsx_pass_data_environment_ptr()));
    check_result(rsx_pass_debug_primitives_create(rsx_pass_data_debug_primitives_ptr()));

    // ...

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _destroy_render_data()
{
    struct rsx_render_data *render_data = rsx_get_render_data();

    // destroy global frame data
    _destroy_shader_resource_frame(&render_data->frame);

    // destroy pass data
    rsx_pass_meshes_destroy(rsx_pass_data_meshes_ptr());
    rsx_pass_gizmos_destroy(rsx_pass_data_gizmos_ptr());
    rsx_pass_environment_destroy(rsx_pass_data_environment_ptr());
    rsx_pass_debug_primitives_destroy(rsx_pass_data_debug_primitives_ptr());
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass : meshes
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e rsx_pass_meshes_create(struct rsx_pass_meshes *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_pass_meshes_destroy(struct rsx_pass_meshes *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass : gizmos
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e rsx_pass_gizmos_create(struct rsx_pass_gizmos *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_pass_gizmos_destroy(struct rsx_pass_gizmos *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass : environment
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e rsx_pass_environment_create(struct rsx_pass_environment *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_pass_environment_destroy(struct rsx_pass_environment *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass : debug primitives
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e rsx_pass_debug_primitives_create(struct rsx_pass_debug_primitives *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_pass_debug_primitives_destroy(struct rsx_pass_debug_primitives *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

error:
    return;
}
