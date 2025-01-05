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

static void _draw_mesh_primitive(struct rsx_mesh_primitive *primitive)
{
    struct rsx_mesh_geometry *geometry = &primitive->mesh->geometry;

    struct softgl_vertex_buffer vertex_buffer = {0};
    vertex_buffer.buffer.data = geometry->vertex_buffer.cpu + primitive->vertices.offset_bytes;
    vertex_buffer.buffer.byte_length = primitive->vertex_stride * primitive->vertices.count;
    vertex_buffer.stride = primitive->vertex_stride;

    struct softgl_vertex_buffer* vertex_buffers[] = {
        &vertex_buffer
    };

    softgl_bind_vertex_buffers(vertex_buffers, 1);

    if (primitive->indices.count > 0)
    {
        struct softgl_index_buffer index_buffer = {0};
        index_buffer.buffer.data = geometry->index_buffer.cpu; // + primitive->indices.offset_bytes; // FIXME
        index_buffer.buffer.byte_length = primitive->indices.count * sizeof(u32); // FIXME 16/32 bit indices

        softgl_bind_index_buffer(&index_buffer);

        softgl_draw_indexed(0, primitive->indices.count);
    }
    else {
        softgl_draw(0, primitive->vertices.count);
    }
}

static void _draw_mesh(struct rsx_mesh *mesh)
{
    struct rsx_mesh_geometry *geometry = &mesh->geometry;

    // static mesh: no need to calculate the mvp for every vertex
    struct shader_data_object *object_data = &mesh->shader_data.data.cpu;
    {
        // do not overwrite an already calculated mvp
        if (!object_data->use_object_mvp)
        {
            struct shader_data_frame *frame_data = &rsx_get_render_data()->frame.data.cpu;

            object_data->mtx_mvp = mat44_mult(mat44_mult(frame_data->mtx_projection, frame_data->mtx_view), object_data->mtx_model);
        }
    }

    // bind object shader data
    softgl_bind_descriptor_set(SOFTGL_DESCRIPTOR_SET_TYPE_OBJECT, object_data);

    // draw primitives
    for (u32 i = 0; i < vector_size(geometry->primitives); i++)
    {
        struct rsx_mesh_primitive *primitive = vector_get(geometry->primitives, i);

        // set material
        struct rsx_material *material = primitive->material;

        softgl_bind_descriptor_set(SOFTGL_DESCRIPTOR_SET_TYPE_MATERIAL, &material->shader_data.data.cpu);
        softgl_bind_pipeline(material->pso.cpu);

        // draw primitive
        _draw_mesh_primitive(primitive);
    }
}

void rcpu_pass_meshes(struct rsx_pass_meshes *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->enabled);

    struct rsx_pass_meshes_priv *priv = &pass_data->priv;

    for (u32 i = 0; i < vector_size(priv->meshes); i++)
    {
        struct rsx_mesh **mesh = vector_get(priv->meshes, i);

        _draw_mesh(*mesh);
    }

error:
    return;
}

void rcpu_pass_gizmos(struct rsx_pass_gizmos *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->enabled);

    struct rsx_pass_gizmos_priv *priv = &pass_data->priv;

    // grid
    if (pass_data->draw_grid) {
        _draw_mesh(priv->mesh.grid);
    }

    // orientation axes
    if (pass_data->draw_orientation_axes)
    {
        // set the new viewport (top right corner)
        struct softgl_viewport vp = rcpu_ptr()->vp;
        priv->calc_axes_viewport(&vp.x, &vp.y, &vp.width, &vp.height);

        softgl_set_viewport(vp);

        // draw axes
        _draw_mesh(priv->mesh.axes);

        // restore the old viewport
        softgl_set_viewport(rcpu_ptr()->vp);
    }

error:
    return;
}

void rcpu_pass_environment(struct rsx_pass_environment *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->enabled);

    // ...

error:
    return;
}

void rcpu_pass_debug_primitives(struct rsx_pass_debug_primitives *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->enabled);

    // ...

error:
    return;
}
