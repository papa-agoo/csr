////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gp.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

void softgl_set_viewport(struct softgl_viewport vp)
{
    check_expr(vp.width > 0 && vp.height > 0);
    check_expr(vp.x < vp.width && vp.y < vp.height);
    check_expr(vp.min_depth != vp.max_depth);

    softgl_state_ptr()->viewport = vp;

error:
    return;
}

void softgl_set_pixelbuffer(struct pixelbuffer *pb)
{
    check_ptr(pb);

    softgl_state_ptr()->pb = pb;

error:
    return;
}

void softgl_bind_pipeline(softgl_pipeline p_pipeline)
{
    struct softgl_pipeline *pipeline = object_pool_get(softgl_storage_ptr()->pipelines, p_pipeline.handle);
    check_ptr(pipeline);

    softgl_binding_ptr().pipeline = pipeline;

error:
    return;
}

void softgl_bind_index_buffer(struct softgl_index_buffer *buffer)
{
    check_ptr(buffer);

    softgl_binding_ptr().index_buffer = buffer;

error:
    return;
}

void softgl_bind_vertex_buffers(struct softgl_vertex_buffer **buffers, u32 buffer_count)
{
    check_ptr(buffers);
    check_expr(buffer_count > 0 && buffer_count <= SOFTGL_VERTEX_BUFFER_BINDING_COUNT);

    for (u32 i = 0; i < buffer_count; i++)
    {
        struct softgl_vertex_buffer *buffer = buffers[i];
        check_ptr(buffer);

        softgl_binding_ptr().vertex_buffers[i] = buffer;
    }

error:
    return;
}

void softgl_draw(u32 first, u32 count)
{
    struct softgl_state *state = softgl_state_ptr();

    check_expr(count > 0);
    check_ptr(state->pb);
    check_ptr(softgl_pso_ptr());

    struct input_assembler* ia = &softgl_gp_ptr()->ia;

    gp_ia_setup(ia, softgl_pso_ptr()->topology, first, count);

    while (gp_ia_seek_stream(&ia->vs))
    {
        gp_process_vertices(softgl_binding_ptr().index_buffer);
        gp_process_primitives();
    }

error:
    return;
}

void softgl_draw_indexed(u32 first, u32 count)
{
    check_ptr(softgl_binding_ptr().index_buffer);

    softgl_draw(first, count);

error:
    return;
}
