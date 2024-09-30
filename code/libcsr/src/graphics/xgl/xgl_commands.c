////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

bool xgl_begin_render_pass(struct xgl_render_pass_info* info)
{
    check_ptr(info);

    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_framebuffer *framebuffer = object_pool_get(storage->framebuffers, info->framebuffer.handle);
    check_ptr(framebuffer);

    xgl_bind_framebuffer_impl(framebuffer->gpu_id);

    f32 *rgba_color = info->clear_values.color.data;
    xgl_clear_color_buffer_impl(framebuffer->gpu_id, 0, rgba_color);

    xgl_clear_depth_stencil_buffer_impl(framebuffer->gpu_id, info->clear_values.depth, info->clear_values.stencil);

    // FIXME make checks

    return true;

error:
    return false;
}

void xgl_end_render_pass()
{
    xgl_reset_pipeline_state_impl();
    xgl_reset_resource_bindings_impl();

    xgl_driver_ptr()->state.pipeline = NULL;

error:
    return;
}

void xgl_set_viewports(u32 count, struct xgl_viewport* viewports)
{
    check_ptr(viewports);

    // rctx->viewport = viewports[0];

    xgl_set_viewport_impl(viewports[0]);

error:
    return;
}

void xgl_set_scissor_rects(u32 count, struct xgl_rect* scissors)
{
    check_ptr(scissors);

    // rctx->scissor = scissors[0];

    xgl_set_scissor_rect_impl(scissors[0]);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

void xgl_bind_pipeline(enum xgl_pipeline_type type, xgl_pipeline p_pipeline)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline.handle);
    check_ptr(pipeline);

    xgl_driver_ptr()->state.pipeline = pipeline;

    xgl_bind_pipeline_impl(pipeline->gpu_id);

error:
    return;
}

// binding relative to descriptor set position (set_idx + n, ...)
static u32 _calc_relative_ds_binding_idx(u32 set_index, u32 slot_count, u32 binding)
{
    // set_index must be one of the xgl_descriptor_set_type elements
    check_expr(set_index < XGL_DESCRIPTOR_SET_TYPE_MAX);

    // at least one slot required
    check_expr(slot_count > 0);

    return (set_index * slot_count) + binding;

error:
    return 0;
}

void xgl_bind_descriptor_set(enum xgl_descriptor_set_type type, xgl_pipeline_layout p_pipeline_layout, xgl_descriptor_set p_set)
{
    check_expr(type < XGL_DESCRIPTOR_SET_TYPE_MAX);

    u32 set_index = type;

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_pipeline_layout *pipeline_layout = object_pool_get(storage->pipeline_layouts, p_pipeline_layout.handle);
    check_ptr(pipeline_layout);

    check_expr(pipeline_layout->ds_layout_count > set_index);

    struct xgl_descriptor_set *set = object_pool_get(storage->descriptor_sets, p_set.handle);
    check_ptr(set);

    struct xgl_descriptor_set_layout *ds_layout_src = object_pool_get(storage->descriptor_set_layouts, set->set_layout.handle);
    check_ptr(ds_layout_src);

    struct xgl_descriptor_set_layout *ds_layout_dst = object_pool_get(storage->descriptor_set_layouts, pipeline_layout->ds_layouts[set_index].handle);
    check_ptr(ds_layout_dst);

    check_expr(ds_layout_src->format != 0);
    check_expr(ds_layout_src->format == ds_layout_dst->format);

    ////////////////////////////////////////

    if (set->buffer_descriptors)
    {
        for (u32 i = 0; i < vector_size(set->buffer_descriptors); i++)
        {
            struct xgl_buffer_descriptor *descriptor = vector_get(set->buffer_descriptors, i);
            check_ptr(descriptor);

            struct xgl_buffer *buffer = object_pool_get(storage->buffers, descriptor->buffer.handle);
            check_ptr(buffer);

            u32 relative_binding = _calc_relative_ds_binding_idx(set_index, XGL_DESCRIPTOR_SET_UB_COUNT, descriptor->binding);

            xgl_bind_uniform_buffer_impl(buffer->gpu_id, relative_binding);
        }
    }

    if (set->texture_descriptors)
    {
        for (u32 i = 0; i < vector_size(set->texture_descriptors); i++)
        {
            struct xgl_texture_descriptor *descriptor = vector_get(set->texture_descriptors, i);
            check_ptr(descriptor);

            u32 relative_binding = _calc_relative_ds_binding_idx(set_index, XGL_DESCRIPTOR_SET_TU_COUNT, descriptor->binding);

            // texture
            struct xgl_texture *texture = object_pool_get(storage->textures, descriptor->texture.handle);
            check_ptr(texture);

            xgl_bind_texture_impl(texture->gpu_id, relative_binding);

            // sampler
            struct xgl_sampler *sampler = object_pool_get(storage->samplers, descriptor->sampler.handle);
            check_ptr(sampler);

            xgl_bind_sampler_impl(sampler->gpu_id, relative_binding);
        }
    }

error:
    return;
}

void xgl_bind_index_buffer(xgl_buffer p_buffer)
{
    // bind_to_render_pass()
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_buffer *buffer = object_pool_get(storage->buffers, p_buffer.handle);
    check_ptr(buffer);

    // FIXME bind only on changed buffer
    xgl_bind_index_buffer_impl(buffer->gpu_id);

error:
    return;
}

void xgl_bind_vertex_buffers(u32 start_at, u32 count, xgl_buffer* p_buffers, u32* offsets, u32* strides)
{
    check_ptr(p_buffers);
    check_ptr(offsets);
    check_ptr(strides);

    // bind_to_render_pass()

    // check_expr(start_at + count <= MAX_VERTEX_BUFFER_BINDINGS);
    check_expr(count > 0);

    struct xgl_storage *storage = xgl_storage_ptr();

    for (u32 i = 0; i < count; i++)
    {
        struct xgl_buffer *buffer = object_pool_get(storage->buffers, p_buffers[i].handle);
        check_ptr(buffer);

        xgl_bind_vertex_buffer_impl(buffer->gpu_id, start_at + i, offsets[i], strides[i]);
    }

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

static void _apply_index_buffer_binding()
{
    // FIXME
}

static void _apply_vertex_buffer_bindings()
{
    // FIXME
}

void xgl_draw(u32 first, u32 count)
{
    if (count == 0) return;

    _apply_vertex_buffer_bindings();

    xgl_draw_impl(first, count);

error:
    return;
}

void xgl_draw_indexed(u32 first, u32 count)
{
    if (count == 0) return;

    _apply_index_buffer_binding();
    _apply_vertex_buffer_bindings();

    xgl_draw_indexed_impl(first, count);

error:
    return;
}
