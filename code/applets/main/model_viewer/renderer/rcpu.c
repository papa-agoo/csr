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
    rcpu_pass_debug_primitives(renderer);

error:
    return;
}

void rcpu_draw_mesh_primitive(struct mesh_primitive *primitive)
{
    struct vector *vertex_buffer_data = primitive->buffer->vertices.cpu;
    struct vector *index_buffer_data = primitive->buffer->indices.cpu;

    struct softgl_vertex_buffer vertex_buffer = {0};
    vertex_buffer.buffer.data = vector_data(vertex_buffer_data);
    vertex_buffer.buffer.byte_length = vector_size(vertex_buffer_data);
    vertex_buffer.stride = primitive->vertex_stride;

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

    // grid
    struct mesh_gizmo *grid = &renderer->gizmo.grid;
    {
        softgl_bind_descriptor_set(SOFTGL_DESCRIPTOR_SET_TYPE_OBJECT, &grid->data);

        softgl_bind_pipeline(cache->pipeline.lines);
        rcpu_draw_mesh_primitive(&grid->primitive);
    }

    // axes
    struct mesh_gizmo *axes = &renderer->gizmo.axes;
    {
        softgl_bind_descriptor_set(SOFTGL_DESCRIPTOR_SET_TYPE_OBJECT, &axes->data);

        // draw the axes gizmo (top right corner)
        {
            struct softgl_viewport vp = rcpu_ptr()->vp;
            renderer_calc_axes_viewport(&vp.x, &vp.y, &vp.width, &vp.height);

            softgl_set_viewport(vp);

            softgl_bind_pipeline(cache->pipeline.lines);
            rcpu_draw_mesh_primitive(&axes->primitive);
        }

        // restore viewport
        softgl_set_viewport(rcpu_ptr()->vp);
    }

error:
    return;
}

static void _draw_primitives(struct renderer *renderer, struct mesh_buffer *buffer, softgl_pipeline pso)
{
    check_ptr(renderer);
    check_ptr(buffer);

    check_quiet(vector_size(buffer->vertices.cpu) > 0);

    struct mesh_primitive mesh = {0};
    mesh.buffer = buffer;
    mesh.vertices.count = vector_size(buffer->vertices.cpu);

    // FIXME >>>
    mesh.vertex_format = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT;
    mesh.vertex_stride = sizeof(struct vertex_1p1c);
    // FIXME <<<

    softgl_bind_pipeline(pso);
    rcpu_draw_mesh_primitive(&mesh);

error:
    return;
}

void rcpu_pass_debug_primitives(struct renderer *renderer)
{
    struct rcpu_cache *cache = &renderer->rcpu->cache;
    struct renderer_shader_data *shader_data = &renderer->shader_data;

    // FIXME
    softgl_bind_descriptor_set(SOFTGL_DESCRIPTOR_SET_TYPE_OBJECT, &shader_data->object.buffer.cpu);

    // draw primitives
    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        _draw_primitives(renderer, &renderer->debug_draw.lines[i], cache->pipeline.lines);
        _draw_primitives(renderer, &renderer->debug_draw.points[i], cache->pipeline.points);

        _draw_primitives(renderer, &renderer->debug_draw.lines_no_depth[i], cache->pipeline.lines_no_depth);
        _draw_primitives(renderer, &renderer->debug_draw.points_no_depth[i], cache->pipeline.points_no_depth);
    }

error:
    return;
}
