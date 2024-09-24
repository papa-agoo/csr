////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rgpu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

void rgpu_set_material(struct material *material)
{
    // copy material data
    {
        struct shader_data_material *cpu = NULL;
        struct shader_data_material *gpu = NULL;

        // ...
    }

    struct material_bindings *bindings = &material->bindings;

    // bind pipeline + descriptors
    // xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, material->effect.pipeline);

    // xgl_bind_descriptor_set(pipelie_layout, XGL_DESCRIPTOR_SET_TYPE_PER_MATERIAL, material_data);
    // xgl_bind_descriptor_set(pipelie_layout, XGL_DESCRIPTOR_SET_TYPE_PER_OBJECT, push_data);
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
