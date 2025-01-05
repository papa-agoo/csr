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

static void _draw_mesh_primitive(struct rsx_mesh_primitive *primitive)
{
    struct rsx_mesh_geometry *geometry = &primitive->mesh->geometry;

    xgl_buffer vertex_buffers[] = {
        geometry->vertex_buffer.gpu,
    };

    u32 first_binding = 0;
    u32 binding_count = COUNT_OF(vertex_buffers);

    u32 vb_offsets[] = {primitive->vertices.offset_bytes};
    u32 vb_strides[] = {primitive->vertex_stride};

    xgl_bind_vertex_buffers(first_binding, binding_count, vertex_buffers, vb_offsets, vb_strides);

    if (primitive->indices.count > 0) {
        xgl_bind_index_buffer(geometry->index_buffer.gpu);
        xgl_draw_indexed(0, primitive->indices.count);
    }
    else {
        xgl_draw(0, primitive->vertices.count);
    }
}

static void _draw_mesh(struct rsx_mesh *mesh)
{
    struct rsx_mesh_geometry *geometry = &mesh->geometry;

    // update object shader data (FIXME: centralised updates in rsx, dirty only)
    struct rsx_uniform_buffer_object *ubo = &mesh->shader_data.data;
    {
        void *gpu_ptr = xgl_map_buffer(ubo->gpu);

        memcpy(gpu_ptr, &ubo->cpu, sizeof(ubo->cpu));
        xgl_unmap_buffer(ubo->gpu);
    }

    // bind object shader data
    struct rsx_shader_resource_binding *mesh_data = &mesh->shader_data.binding;
    xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_OBJECT, mesh_data->pipeline_layout, mesh_data->ds);

    // draw primitives
    for (u32 i = 0; i < vector_size(geometry->primitives); i++)
    {
        struct rsx_mesh_primitive *primitive = vector_get(geometry->primitives, i);

        // set material
        struct rsx_material *material = primitive->material;

        // xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_MATERIAL, material->shader_data.binding.pipeline_layout, material->shader_data.binding.ds);
        xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, material->pso.gpu);

        // draw primitive
        _draw_mesh_primitive(primitive);
    }

error:
    return;
}

void rgpu_pass_meshes(struct rsx_pass_meshes *pass_data)
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

void rgpu_pass_gizmos(struct rsx_pass_gizmos *pass_data)
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
        struct xgl_viewport vp = rgpu_ptr()->vp;
        priv->calc_axes_viewport(&vp.x, &vp.y, &vp.width, &vp.height);

        xgl_set_viewports(1, &vp);

        // draw axes
        _draw_mesh(priv->mesh.axes);

        // restore the old viewport
        xgl_set_viewports(1, &rgpu_ptr()->vp);
    }

error:
    return;
}

void rgpu_pass_environment(struct rsx_pass_environment *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->enabled);

    // ...

error:
    return;
}

void rgpu_pass_debug_primitives(struct rsx_pass_debug_primitives *pass_data)
{
    check_ptr(pass_data);
    check_quiet(pass_data->enabled);

    // ...

error:
    return;
}
