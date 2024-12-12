////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rcpu_priv.h"
#include "rsx_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct rcpu g_rcpu = {0};

#define rcpu_ptr() (&g_rcpu)
#define rcpu_cache_ptr() (&rcpu_ptr()->cache)

////////////////////////////////////////////////////////////

struct rcpu* rcpu_create()
{
    check_result(softgl_init_driver());
    check_result(rcpu_create_cache(rcpu_cache_ptr()));

    return &g_rcpu;

error:
    return NULL;
}

void rcpu_destroy()
{
    rcpu_destroy_cache(rcpu_cache_ptr());

    softgl_quit_driver();

error:
    return;
}

void rcpu_tick(struct pixelbuffer *pb, struct softgl_viewport vp)
{
    check_ptr(pb);

    struct rsx_render_data *render_data = rsx_get_render_data();
    struct rsx_uniform_buffer_frame *frame_data = &render_data->frame.data;

    // set the viewport for this frame
    rcpu_ptr()->vp = vp;

    softgl_set_viewport(vp);
    softgl_set_pixelbuffer(pb);

    // bind frame data
    softgl_bind_descriptor_set(SOFTGL_DESCRIPTOR_SET_TYPE_FRAME, &frame_data->cpu);

    // draw passes
    rcpu_pass_meshes(&render_data->pass.meshes);
    rcpu_pass_gizmos(&render_data->pass.gizmos);
    rcpu_pass_environment(&render_data->pass.environment);
    rcpu_pass_debug_primitives(&render_data->pass.debug_primitives);

error:
    return;
}

// static void _draw_mesh_primitive(struct mesh_primitive *primitive)
// {
//     struct vector *vertex_buffer_data = primitive->buffer->vertices.cpu;
//     struct vector *index_buffer_data = primitive->buffer->indices.cpu;

//     struct softgl_vertex_buffer vertex_buffer = {0};
//     vertex_buffer.buffer.data = vector_data(vertex_buffer_data);
//     vertex_buffer.buffer.byte_length = vector_size(vertex_buffer_data);
//     vertex_buffer.stride = primitive->vertex_stride;

//     struct softgl_vertex_buffer* vertex_buffers[] = {
//         &vertex_buffer
//     };

//     softgl_bind_vertex_buffers(vertex_buffers, 1);

//     if (primitive->indices.count > 0)
//     {
//         struct softgl_index_buffer index_buffer = {0};
//         index_buffer.buffer.data = vector_data(index_buffer_data);
//         index_buffer.buffer.byte_length = vector_size(index_buffer_data);

//         softgl_bind_index_buffer(&index_buffer);

//         softgl_draw_indexed(primitive->indices.start, primitive->indices.count);
//     }
//     else {
//         softgl_draw(primitive->vertices.start, primitive->vertices.count);
//     }
// }

void rcpu_pass_meshes(struct rsx_pass_meshes *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->base.enabled);

    // ...

error:
    return;
}

void rcpu_pass_gizmos(struct rsx_pass_gizmos *pass_data)
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

void rcpu_pass_environment(struct rsx_pass_environment *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->base.enabled);

    // ...

error:
    return;
}

void rcpu_pass_debug_primitives(struct rsx_pass_debug_primitives *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->base.enabled);

    // ...

error:
    return;
}
