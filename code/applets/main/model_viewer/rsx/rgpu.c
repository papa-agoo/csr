////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rgpu_priv.h"
#include "rsx_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct rgpu g_rgpu = {0};

#define rgpu_ptr() (&g_rgpu)
#define rgpu_cache_ptr() (&rgpu_ptr()->cache)

////////////////////////////////////////////////////////////

struct rgpu* rgpu_create()
{
    check_result(rgpu_create_cache(rgpu_cache_ptr()));

    return &g_rgpu;

error:
    return NULL;
}

void rgpu_destroy()
{
    rgpu_destroy_cache(rgpu_cache_ptr());
}

void rgpu_tick(struct xgl_viewport vp)
{
    struct rsx_render_data *render_data = rsx_get_render_data();
    struct rsx_shader_resource_binding *frame_data = &render_data->frame.binding;

    // set the viewport for this frame
    rgpu_ptr()->vp = vp;

    // bind frame data
    xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_FRAME, frame_data->pipeline_layout, frame_data->ds);

    // draw passes
    rgpu_pass_meshes(&render_data->pass.meshes);
    rgpu_pass_gizmos(&render_data->pass.gizmos);
    rgpu_pass_environment(&render_data->pass.environment);
    rgpu_pass_debug_primitives(&render_data->pass.debug_primitives);

error:
    return;
}

// static void _draw_mesh_primitive(struct mesh_primitive *primitive)
// {
//     xgl_buffer vertex_buffers[] = {
//         primitive->buffer->vertices.gpu,
//     };

//     u32 first_binding = 0;
//     u32 binding_count = COUNT_OF(vertex_buffers);

//     u32 vb_offsets[] = {0};
//     u32 vb_strides[] = {primitive->vertex_stride};

//     xgl_bind_vertex_buffers(first_binding, binding_count, vertex_buffers, vb_offsets, vb_strides);

//     if (primitive->indices.count > 0) {
//         xgl_bind_index_buffer(primitive->buffer->indices.gpu);
//         xgl_draw_indexed(primitive->indices.start, primitive->indices.count);
//     }
//     else {
//         xgl_draw(primitive->vertices.start, primitive->vertices.count);
//     }
// }

void rgpu_pass_meshes(struct rsx_pass_meshes *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->base.enabled);

    // ...

error:
    return;
}

void rgpu_pass_gizmos(struct rsx_pass_gizmos *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->base.enabled);

    // grid
    if (pass_data->draw_grid)
    {
        // ...
    }

    // orientation axes
    if (pass_data->draw_orientation_axes)
    {
        // ...
    }

error:
    return;
}

void rgpu_pass_environment(struct rsx_pass_environment *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->base.enabled);

    // ...

error:
    return;
}

void rgpu_pass_debug_primitives(struct rsx_pass_debug_primitives *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->base.enabled);

    // ...

error:
    return;
}
