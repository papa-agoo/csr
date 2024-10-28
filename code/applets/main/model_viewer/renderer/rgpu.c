////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rgpu_priv.h"
#include "renderer_priv.h"

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

    check_expr(rgpu_create_cache(rgpu_cache_ptr()) == RC_SUCCESS);

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
    struct renderer_shader_data *shader_data = &renderer->shader_data;

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
    rgpu_pass_debug_draw(renderer);

error:
    return;
}

void rgpu_draw_mesh_primitive(struct mesh_primitive *primitive)
{
    xgl_buffer vertex_buffers[] = {
        primitive->buffer->gpu.vertices,
    };

    u32 first_binding = 0;
    u32 binding_count = COUNT_OF(vertex_buffers);

    u32 vb_offsets[] = {0};
    u32 vb_strides[] = {primitive->buffer->vertex_stride};

    xgl_bind_vertex_buffers(first_binding, binding_count, vertex_buffers, vb_offsets, vb_strides);

    if (primitive->indices.count > 0) {
        xgl_bind_index_buffer(primitive->buffer->gpu.indices);
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
    struct renderer_shader_data *shader_data = &renderer->shader_data;

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
    struct renderer_shader_data *shader_data = &renderer->shader_data;

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
    struct renderer_shader_data *shader_data = &renderer->shader_data;

    // bind pass data
    xgl_bind_descriptor_set(XGL_DESCRIPTOR_SET_TYPE_PASS, cache->pipeline_layout.main, shader_data->pass_main.ds);

    // axes
    struct mesh_gizmo *axes = &renderer->gizmo.axes;
    {
        // save current viewport
        struct xgl_viewport vp = renderer->rgpu->vp;

        // draw the axes gizmo (top right corner)
        {
            // >>> FIXME
            f32 size_wh = vp.width * 0.03;
            f32 margin_xy = vp.height * 0.01;

            struct xgl_viewport vp_gizmo = vp;
            vp_gizmo.width = size_wh;
            vp_gizmo.height = size_wh;
            vp_gizmo.x = vp.width - size_wh - margin_xy;
            vp_gizmo.y = vp.height - size_wh - margin_xy;
            // <<< FIXME

            xgl_set_viewports(1, &vp_gizmo);

            xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, cache->pipeline.lines_fat);
            rgpu_draw_mesh_primitive(&axes->primitive);
        }

        // restore viewport
        xgl_set_viewports(1, &vp);
    }

    // grid
    struct mesh_gizmo *grid = &renderer->gizmo.grid;
    {
        xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, cache->pipeline.lines);
        rgpu_draw_mesh_primitive(&grid->primitive);
    }

error:
    return;
}

void rgpu_pass_debug_draw(struct renderer *renderer)
{
    // ...
}
