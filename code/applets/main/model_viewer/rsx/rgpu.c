////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rgpu_priv.h"
#include "rsx_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct rgpu g_rgpu = {0};

#define rgpu_ptr() (&g_rgpu)
#define rgpu_cache_ptr() (&rgpu_ptr()->cache)

struct rgpu* rgpu_create()
{
    // rgpu_build_shader_source_files()
    {
        //  - shader_stage_xxx foo = {0};
        //
        //  - add_define(foo, "USE_FOO")
        //  - add_define(foo, "ENABLE_BAR")
        //  - add_source_file(foo, "common.inc.glsl")
        //  - add_source_file(foo, "vertex_color.vs.glsl")
        //
        //  - shader_create_stage(...)
    }

    check_result(rgpu_create_cache(rgpu_cache_ptr()));

    return &g_rgpu;

error:
    return NULL;
}

void rgpu_destroy()
{
    rgpu_destroy_cache(&rgpu_ptr()->cache);
}

// TODO rgpu_tick()
//
// copy_frame_shader_data
// bind_ds_frame
//
// bind_ds_pass
// for material in materials[]
//      bind_ds_material
//      bind_pipeline
//      for primitive in primitives[]
//          copy_object_shader_data
//          bind_ds_object
//

void rgpu_tick(struct renderer *renderer, struct xgl_viewport vp)
{
    check_ptr(renderer);

    struct rgpu_cache *cache = rgpu_cache_ptr();
    struct rsx_shader_data *shader_data = &renderer->shader_data;

    // set the viewport for this frame
    rgpu_ptr()->vp = vp;

    // copy frame data
    {
        struct shader_data_frame *cpu = &shader_data->frame.buffer.cpu;
        struct shader_data_frame *gpu = xgl_map_buffer(shader_data->frame.buffer.gpu);

        memcpy(gpu, cpu, sizeof(struct shader_data_frame));
        xgl_unmap_buffer(shader_data->frame.buffer.gpu);
    }

    // bind frame data
    xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_FRAME, cache->pipeline_layout.main, shader_data->frame.ds);

    // main pass (meshes)
    // struct model *model = model_viewer_get_model();

    // if (model) {
    //     rgpu_pass_main(renderer, model->resources.mesh.meshes);
    // }

    rgpu_pass_meshes(renderer, NULL);
    rgpu_pass_gizmos(renderer);
    rgpu_pass_debug_primitives(renderer);

error:
    return;
}

void rgpu_draw_mesh_primitive(struct mesh_primitive *primitive)
{
    xgl_buffer vertex_buffers[] = {
        primitive->buffer->vertices.gpu,
    };

    u32 first_binding = 0;
    u32 binding_count = COUNT_OF(vertex_buffers);

    u32 vb_offsets[] = {0};
    u32 vb_strides[] = {primitive->vertex_stride};

    xgl_bind_vertex_buffers(first_binding, binding_count, vertex_buffers, vb_offsets, vb_strides);

    if (primitive->indices.count > 0) {
        xgl_bind_index_buffer(primitive->buffer->indices.gpu);
        xgl_draw_indexed(primitive->indices.start, primitive->indices.count);
    }
    else {
        xgl_draw(primitive->vertices.start, primitive->vertices.count);
    }
}

void rgpu_pass_environment(struct renderer *renderer)
{
    check_ptr(renderer);

    struct rgpu_cache *cache = rgpu_cache_ptr();
    struct rsx_shader_data *shader_data = &renderer->shader_data;

    // bind pass data
    xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_PASS, cache->pipeline_layout.environment, shader_data->pass_environment.ds);

    // ...

error:
    return;
}

void rgpu_pass_meshes(struct renderer *renderer, struct vector *meshes)
{
    check_ptr(renderer);
    check_quiet(meshes);

    struct rgpu_cache *cache = rgpu_cache_ptr();
    struct rsx_shader_data *shader_data = &renderer->shader_data;

    // bind pass data
    xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_PASS, cache->pipeline_layout.main, shader_data->pass_main.ds);

    // draw meshes
    for (u32 i = 0; i < vector_size(meshes); i++)
    {
        struct mesh *mesh = vector_get(meshes, i);

        // update object shader data
        {
            struct shader_data_object *cpu = &mesh->shader_data.buffer.cpu;
            struct shader_data_object *gpu = xgl_map_buffer(mesh->shader_data.buffer.gpu);

            memcpy(gpu, cpu, sizeof(struct shader_data_object));
            xgl_unmap_buffer(mesh->shader_data.buffer.gpu);
        }

        // draw primitives
        for (u32 i = 0; i < vector_size(mesh->primitives); i++)
        {
            struct mesh_primitive *primitive = vector_get(mesh->primitives, i);

            struct material *material = primitive->material;

            xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_MATERIAL, material->effect.pipeline_layout, material->shader_data.ds);
            xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_OBJECT, cache->pipeline_layout.main, mesh->shader_data.ds);

            xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, material->effect.pipeline);

            rgpu_draw_mesh_primitive(primitive);
        }
    }

error:
    return;
}

void rgpu_pass_gizmos(struct renderer *renderer)
{
    struct rgpu_cache *cache = rgpu_cache_ptr();
    struct rsx_shader_data *shader_data = &renderer->shader_data;

    // bind pass data
    xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_PASS, cache->pipeline_layout.main, shader_data->pass_main.ds);

    // bind object data
    xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_OBJECT, cache->pipeline_layout.main, shader_data->object.ds);

    // grid
    struct mesh_gizmo *grid = &renderer->gizmo.grid;
    {
        // FIXME streamline
        struct shader_data_object *cpu = &grid->data;
        {
            struct shader_data_object *gpu = xgl_map_buffer(shader_data->object.buffer.gpu);

            memcpy(gpu, cpu, sizeof(struct shader_data_object));
            xgl_unmap_buffer(shader_data->object.buffer.gpu);
        }

        xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, cache->pipeline.lines[PRIMITIVE_SIZE_NORMAL]);
        rgpu_draw_mesh_primitive(&grid->primitive);
    }

    // axes
    struct mesh_gizmo *axes = &renderer->gizmo.axes;
    {
        // FIXME streamline
        struct shader_data_object *cpu = &axes->data;
        {
            struct shader_data_object *gpu = xgl_map_buffer(shader_data->object.buffer.gpu);

            memcpy(gpu, cpu, sizeof(struct shader_data_object));
            xgl_unmap_buffer(shader_data->object.buffer.gpu);
        }

        // draw the axes gizmo (top right corner)
        {
            struct xgl_viewport vp = rgpu_ptr()->vp;
            rsx_calc_axes_viewport(&vp.x, &vp.y, &vp.width, &vp.height);

            xgl_set_viewports(1, &vp);

            xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, cache->pipeline.lines_no_depth[PRIMITIVE_SIZE_THICK]);
            rgpu_draw_mesh_primitive(&axes->primitive);
        }

        // restore viewport
        xgl_set_viewports(1, &rgpu_ptr()->vp);
    }

error:
    return;
}

static void _draw_primitives(struct renderer *renderer, struct mesh_buffer *buffer, xgl_pipeline pso)
{
    check_ptr(renderer);
    check_ptr(buffer);

    check_quiet(vector_size(buffer->vertices.cpu) > 0);

    // update gpu buffer
    check_result(xgl_update_buffer(buffer->vertices.gpu, 0, vector_byte_length(buffer->vertices.cpu), vector_data(buffer->vertices.cpu)));

    // draw primitives
    struct mesh_primitive mesh = {0};
    mesh.buffer = buffer;
    mesh.vertices.count = vector_size(buffer->vertices.cpu);

    // FIXME >>>
    mesh.vertex_format = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT;
    mesh.vertex_stride = sizeof(struct vertex_1p1c);
    // FIXME <<<

    xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, pso);
    rgpu_draw_mesh_primitive(&mesh);

error:
    return;
}

void rgpu_pass_debug_primitives(struct renderer *renderer)
{
    struct rgpu_cache *cache = rgpu_cache_ptr();
    struct rsx_shader_data *shader_data = &renderer->shader_data;

    xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_OBJECT, cache->pipeline_layout.main, shader_data->object.ds);

    // FIXME streamline
    struct shader_data_object *cpu = &shader_data->object.buffer.cpu;
    {
        struct shader_data_object *gpu = xgl_map_buffer(shader_data->object.buffer.gpu);

        memcpy(gpu, cpu, sizeof(struct shader_data_object));
        xgl_unmap_buffer(shader_data->object.buffer.gpu);
    }

    // draw primitives
    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        _draw_primitives(renderer, &renderer->debug_draw.lines[i], cache->pipeline.lines[i]);
        _draw_primitives(renderer, &renderer->debug_draw.points[i], cache->pipeline.points[i]);

        _draw_primitives(renderer, &renderer->debug_draw.lines_no_depth[i], cache->pipeline.lines_no_depth[i]);
        _draw_primitives(renderer, &renderer->debug_draw.points_no_depth[i], cache->pipeline.points_no_depth[i]);
    }
}
