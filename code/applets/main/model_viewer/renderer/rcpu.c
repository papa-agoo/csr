////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rcpu_priv.h"
#include "renderer_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct rcpu g_rcpu = {0};

#define rcpu_ptr() (&g_rcpu)
#define rcpu_cache_ptr() (&rcpu_ptr()->cache)

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

void rcpu_tick(struct renderer *renderer, struct pixelbuffer *pb, struct softgl_viewport vp)
{
    check_ptr(renderer);
    check_ptr(pb);

    struct rcpu_cache *cache = rcpu_cache_ptr();
    struct renderer_shader_data *shader_data = &renderer->shader_data;

    // set the viewport for this frame
    rcpu_ptr()->vp = vp;

    softgl_set_viewport(vp);
    softgl_set_pixelbuffer(pb);

    // bind frame data
    softgl_bind_descriptor_set(SOFTGL_DESCRIPTOR_SET_TYPE_FRAME, &shader_data->frame.buffer.cpu);

    rcpu_pass_meshes(renderer, NULL);
    rcpu_pass_gizmos(renderer);

error:
    return;
}

void rcpu_draw_mesh_primitive(struct mesh_primitive *primitive)
{
    struct vector *vertex_buffer_data = primitive->buffer->cpu.vertices;
    struct vector *index_buffer_data = primitive->buffer->cpu.indices;

    struct softgl_vertex_buffer vertex_buffer = {0};
    vertex_buffer.buffer.data = vector_data(vertex_buffer_data);
    vertex_buffer.buffer.byte_length = vector_size(vertex_buffer_data);
    vertex_buffer.stride = primitive->buffer->vertex_stride;

    struct softgl_vertex_buffer* vertex_buffers[] = {
        &vertex_buffer
    };

    softgl_bind_vertex_buffers(vertex_buffers, 1);

    if (primitive->indices.count > 0)
    {
        struct softgl_index_buffer index_buffer = {0};
        index_buffer.buffer.data = vector_data(index_buffer_data);
        index_buffer.buffer.byte_length = vector_size(index_buffer_data);

        softgl_bind_index_buffer(&index_buffer);

        softgl_draw_indexed(primitive->indices.start, primitive->indices.count);
    }
    else {
        softgl_draw(primitive->vertices.start, primitive->vertices.count);
    }
}

void rcpu_pass_meshes(struct renderer *renderer, struct vector *meshes)
{
    check_ptr(renderer);
    check_quiet(meshes);

    struct rcpu_cache *cache = rcpu_cache_ptr();
    struct renderer_shader_data *shader_data = &renderer->shader_data;

    // ...

error:
    return;
}

void rcpu_pass_gizmos(struct renderer *renderer)
{
    struct rcpu_cache *cache = rcpu_cache_ptr();
    struct renderer_shader_data *shader_data = &renderer->shader_data;

    // bind pass data
    softgl_bind_descriptor_set(SOFTGL_DESCRIPTOR_SET_TYPE_PASS, &shader_data->pass_main.buffer.cpu);

    softgl_bind_pipeline(cache->pipeline.lines);

    // axes
    struct mesh_gizmo *axes = &renderer->gizmo.axes;
    {
        // save current viewport
        struct softgl_viewport vp = rcpu_ptr()->vp;

        // draw the axes gizmo (top right corner)
        {
            // >>> FIXME
            f32 size_wh = vp.width * 0.03;
            f32 margin_xy = vp.height * 0.01;

            struct softgl_viewport vp_gizmo = vp;
            vp_gizmo.width = size_wh;
            vp_gizmo.height = size_wh;
            vp_gizmo.x = vp.width - size_wh - margin_xy;
            vp_gizmo.y = vp.height - size_wh - margin_xy;
            // <<< FIXME

            softgl_set_viewport(vp_gizmo);

            softgl_bind_pipeline(cache->pipeline.lines);
            rcpu_draw_mesh_primitive(&axes->primitive);
        }

        // restore viewport
        softgl_set_viewport(vp);
    }

    // grid
    struct mesh_gizmo *grid = &renderer->gizmo.grid;
    {
        softgl_bind_pipeline(cache->pipeline.lines);
        rcpu_draw_mesh_primitive(&grid->primitive);
    }

error:
    return;
}

void rcpu_pass_debug_draw(struct renderer *renderer)
{
    struct rcpu_cache *cache = &renderer->rcpu->cache;
    struct renderer_shader_data *shader_data = &renderer->shader_data;

    // ...

error:
    return;
}
