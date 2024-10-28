////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME move to core?
static inline u32 hash_fnv1a_32(char *s, size_t count)
{
	return ((count ? hash_fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}

#define _swapchain_current_rt(s) (&(s)->rt[(s)->rt_current_idx])

static result_e _create_render_target(struct xgl_render_target *rt, u32 width, u32 height)
{
    result_e result = RC_FAILURE;

    ////////////////////////////////////////

    // color attachment
    {
        struct xgl_texture_create_info info = {0};
        info.type = XGL_TEXTURE_TYPE_2D;
        info.format = XGL_TEXTURE_FORMAT_RGBA;
        info.usage_flags = XGL_TEXTURE_USAGE_COLOR_ATTACHMENT_BIT;
        info.width = width;
        info.height = height;
        info.sample_count = 1;
        info.array_layer_count = 1;

        result = xgl_create_texture(&info, &rt->color_buffer);
        check_result(result, "could not create color buffer");
    }

    // depth stencil attachment
    {
        struct xgl_texture_create_info info = {0};
        info.type = XGL_TEXTURE_TYPE_2D;
        info.format = XGL_TEXTURE_FORMAT_D24_S8;
        info.usage_flags = XGL_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        info.width = width;
        info.height = height;
        info.sample_count = 1;
        info.array_layer_count = 1;

        result = xgl_create_texture(&info, &rt->depth_stencil_buffer);
        check_result(result, "could not create depth_stencil buffer");
    }

    ////////////////////////////////////////

    xgl_texture attachments[] = {
        rt->color_buffer,
        rt->depth_stencil_buffer,
    };

    struct xgl_framebuffer_create_info info = {0};
    info.width = width;
    info.height = height;
    info.attachments = attachments;
    info.attachment_count = COUNT_OF(attachments);

    return xgl_create_framebuffer(&info, &rt->framebuffer);

error:
    return RC_FAILURE;
}

result_e xgl_create_swapchain(struct xgl_swapchain_create_info *info, xgl_swapchain *p_swapchain)
{
    check_ptr(info);
    check_ptr(p_swapchain);

    ////////////////////////////////////////

    result_e result = RC_FAILURE;

    struct xgl_swapchain swapchain = {0};
    swapchain.width = info->width;
    swapchain.height = info->height;

    result = _create_render_target(&swapchain.rt[0], swapchain.width, swapchain.height);
    check_result(result, "could not create render target 0");

    result = _create_render_target(&swapchain.rt[1], swapchain.width, swapchain.height);
    check_result(result, "could not create render target 1");

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    p_swapchain->handle = object_pool_alloc(storage->swapchains, &swapchain);
    check_guid(p_swapchain->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_destroy_swapchain(xgl_swapchain p_swapchain)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_swapchain *swapchain = object_pool_get(storage->swapchains, p_swapchain.handle);
    check_ptr(swapchain);

    u32 rt_count = 2; // FIXME

    for (u32 i = 0; i < rt_count; i++)
    {
        struct xgl_render_target *rt = &swapchain->rt[i];

        xgl_destroy_texture(rt->color_buffer);
        xgl_destroy_texture(rt->depth_stencil_buffer);

        xgl_destroy_framebuffer(rt->framebuffer);
    }

    object_pool_free(storage->swapchains, p_swapchain.handle);

error:
    return;
}

void xgl_resize_swapchain(xgl_swapchain p_swapchain, u32 width, u32 height)
{
    clog_error("not impl. yet");

    check_expr(width > 0);
    check_expr(height > 0);

error:
    return;
}

xgl_framebuffer xgl_get_swapchain_framebuffer(xgl_swapchain p_swapchain)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_swapchain *swapchain = object_pool_get(storage->swapchains, p_swapchain.handle);
    check_ptr(swapchain);

    struct xgl_render_target *rt = _swapchain_current_rt(swapchain);

    return rt->framebuffer;

error:
    return (xgl_framebuffer){.handle = make_guid(0)};
}

void xgl_present_swapchain_framebuffer(xgl_swapchain p_swapchain)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_swapchain *swapchain = object_pool_get(storage->swapchains, p_swapchain.handle);
    check_ptr(swapchain);

    struct xgl_render_target *rt = _swapchain_current_rt(swapchain);

    struct xgl_framebuffer *framebuffer = object_pool_get(storage->framebuffers, rt->framebuffer.handle);
    check_ptr(framebuffer);

    xgl_present_framebuffer_impl(framebuffer->gpu_id);

    swapchain->rt_current_idx =! swapchain->rt_current_idx;

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

result_e xgl_create_framebuffer(struct xgl_framebuffer_create_info* info, xgl_framebuffer* p_framebuffer)
{
    check_ptr(info);
    check_ptr(p_framebuffer);

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_framebuffer framebuffer = {0};

    framebuffer.width = info->width;
    framebuffer.height = info->height;

    if (info->attachment_count > 0)
    {
        framebuffer.attachments = vector_create(info->attachment_count, sizeof(xgl_texture));
        check_ptr(framebuffer.attachments);

        for (u32 i = 0; i < info->attachment_count; i++)
        {
            xgl_texture attachment = info->attachments[i];
            check_guid(attachment.handle);

            struct xgl_texture *texture = object_pool_get(storage->textures, attachment.handle);
            check_ptr(texture);

            vector_push_back(framebuffer.attachments, texture->gpu_id);
        }
    }

    framebuffer.gpu_id = xgl_create_framebuffer_impl(framebuffer.attachments);
    check_guid(framebuffer.gpu_id);

    ////////////////////////////////////////

    p_framebuffer->handle = object_pool_alloc(storage->framebuffers, &framebuffer);
    check_guid(p_framebuffer->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_destroy_framebuffer(xgl_framebuffer p_framebuffer)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_framebuffer *framebuffer = object_pool_get(storage->framebuffers, p_framebuffer.handle);
    check_ptr(framebuffer);

    xgl_destroy_framebuffer_impl(framebuffer->gpu_id);

    if (framebuffer->attachments) {
        vector_destroy(framebuffer->attachments);
    }

    object_pool_free(storage->framebuffers, p_framebuffer.handle);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

result_e xgl_create_buffer(struct xgl_buffer_create_info* info, xgl_buffer* p_buffer)
{
    check_ptr(info);
    check_ptr(p_buffer);

    // FIXME handle usage flags properly! :)
    check_expr(info->usage_flags != XGL_BUFFER_USAGE_UNKNOWN);
    check_expr(info->byte_length > 0);

    ////////////////////////////////////////

    struct xgl_buffer buffer = {0};
    buffer.usage_flags = info->usage_flags;
    buffer.byte_length = info->byte_length;
    buffer.is_currently_mapped = false;

    buffer.gpu_id = xgl_create_buffer_impl(info->usage_flags, info->byte_length, info->data);
    check_guid(buffer.gpu_id);

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    p_buffer->handle = object_pool_alloc(storage->buffers, &buffer);
    check_guid(p_buffer->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_destroy_buffer(xgl_buffer p_buffer)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_buffer *buffer = object_pool_get(storage->buffers, p_buffer.handle);
    check_ptr(buffer);

    if (buffer->is_currently_mapped) {
        xgl_unmap_buffer(p_buffer);
    }

    xgl_destroy_buffer_impl(buffer->gpu_id);

    object_pool_free(storage->buffers, p_buffer.handle);

error:
    return;
}

result_e xgl_update_buffer(xgl_buffer p_buffer, u32 offset, u32 byte_length, void *data)
{
    check_expr(byte_length > 0);

    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_buffer *buffer = object_pool_get(storage->buffers, p_buffer.handle);
    check_ptr(buffer);

    check_expr(!buffer->is_currently_mapped);
    check_expr(buffer->byte_length >= offset + byte_length);

    return xgl_update_buffer_impl(buffer->gpu_id, offset, byte_length, data);

error:
    return RC_FAILURE;
}

void* xgl_map_buffer(xgl_buffer p_buffer)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_buffer *buffer = object_pool_get(storage->buffers, p_buffer.handle);
    check_ptr(buffer);

    check_expr(!buffer->is_currently_mapped);

    void *ptr = xgl_map_buffer_impl(buffer->gpu_id, 0, buffer->byte_length);
    check(ptr, "could not map buffer");

    buffer->is_currently_mapped = true;

    return ptr;

error:
    return NULL;
}

void xgl_unmap_buffer(xgl_buffer p_buffer)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_buffer *buffer = object_pool_get(storage->buffers, p_buffer.handle);
    check_ptr(buffer);

    check_expr(buffer->is_currently_mapped);

    xgl_unmap_buffer_impl(buffer->gpu_id);

    buffer->is_currently_mapped = false;

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

result_e xgl_create_sampler(struct xgl_sampler_desc* desc, xgl_sampler* p_sampler)
{
    check_ptr(desc);
    check_ptr(p_sampler);

    if (!string_is_valid(desc->name)) {
        desc->name = make_string("<unnamed sampler>");
    }

    ////////////////////////////////////////

    struct xgl_sampler sampler = {0};

    sampler.name = desc->name;

    sampler.min_filter = desc->min_filter;
    sampler.mag_filter = desc->mag_filter;

    sampler.addr_mode_u = desc->addr_mode_u;
    sampler.addr_mode_v = desc->addr_mode_v;
    sampler.addr_mode_w = desc->addr_mode_w;

    for (u32 i = 0; i < 4; i++) {
        sampler.border_color[i] = desc->border_color[i];
    }

    sampler.mipmap_mode = desc->mipmap_mode;
    sampler.mip_lod_bias = desc->mip_lod_bias;
    sampler.min_lod = desc->min_lod;
    sampler.max_lod = desc->max_lod;

    sampler.max_anisotropy = desc->max_anisotropy;

    sampler.gpu_id = xgl_create_sampler_impl(desc);
    check_guid(sampler.gpu_id);

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    p_sampler->handle = object_pool_alloc(storage->samplers, &sampler);
    check_guid(p_sampler->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_destroy_sampler(xgl_sampler p_sampler)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_sampler *sampler = object_pool_get(storage->samplers, p_sampler.handle);
    check_ptr(sampler);

    xgl_destroy_sampler_impl(sampler->gpu_id);

    object_pool_free(storage->samplers, p_sampler.handle);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

result_e xgl_create_texture(struct xgl_texture_create_info* info, xgl_texture* p_texture)
{
    check_ptr(info);
    check_ptr(p_texture);

    check_expr(info->type != XGL_TEXTURE_TYPE_UNKNOWN);
    check_expr(info->format != XGL_TEXTURE_FORMAT_UNKNOWN);

    // FIXME handle usage flags properly! :)
    check_expr(info->usage_flags != XGL_TEXTURE_USAGE_UNKNOWN);

    check_expr(info->width > 0);
    check_expr(info->height > 0);

    ////////////////////////////////////////

    struct xgl_texture texture = {0};

    texture.type = info->type;
    texture.format = info->format;

    texture.usage_flags = info->usage_flags;

    texture.width = info->width;
    texture.height = info->height;

    texture.mip_level_count = (info->mip_level_count > 0) ? info->mip_level_count : 1;
    texture.array_layer_count = (info->array_layer_count > 0) ? info->array_layer_count : 1;
    texture.sample_count = (info->sample_count > 0) ? info->sample_count : 1;

    texture.gpu_id = xgl_create_texture_impl(texture.type, texture.sample_count, texture.usage_flags);
    check_guid(texture.gpu_id);

    result_e result = xgl_alloc_texture_storage_impl(texture.gpu_id, texture.format,
        texture.mip_level_count, texture.width, texture.height, texture.array_layer_count);

    texture.storage_allocated = R_SUCCESS(result);

    if (!texture.storage_allocated) {
        clog_trace("could not allocate texture storage");
    }

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    p_texture->handle = object_pool_alloc(storage->textures, &texture);
    check_guid(p_texture->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_destroy_texture(xgl_texture p_texture)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_texture *texture = object_pool_get(storage->textures, p_texture.handle);
    check_ptr(texture);

    xgl_destroy_texture_impl(texture->gpu_id);

    object_pool_free(storage->textures, p_texture.handle);

error:
    return;
}

result_e xgl_update_texture(xgl_texture p_texture, u32 layer, u32 width, u32 height, u8 *data)
{
    check_ptr(data);

    check_expr(width > 0);
    check_expr(height > 0);

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_texture *texture = object_pool_get(storage->textures, p_texture.handle);
    check_ptr(texture);

    check_expr(texture->storage_allocated);
    check_expr(texture->array_layer_count > layer);

    result_e result = xgl_update_texture_impl(texture->gpu_id, layer, width, height, data);
    check_result(result, "could not update texture data");

    if (texture->mip_level_count > 1)
    {
        if (texture->array_layer_count == 1)
        {
            result = xgl_generate_mipmaps_impl(texture->gpu_id);

            if (!R_SUCCESS(result)) {
                clog_trace("could not generate texture mipmaps");
            }
        }
        else {
            clog_trace("texture mipmap generation not supported for layered textures yet");
        }
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void* xgl_map_texture(xgl_texture p_texture)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_texture *texture = object_pool_get(storage->textures, p_texture.handle);
    check_ptr(texture);

    check_expr(!texture->is_currently_mapped);

    void *ptr = xgl_map_texture_impl(texture->gpu_id);
    check(ptr, "could not map texture");

    texture->is_currently_mapped = true;

    return ptr;

error:
    return NULL;
}

void xgl_unmap_texture(xgl_texture p_texture)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_texture *texture = object_pool_get(storage->textures, p_texture.handle);
    check_ptr(texture);

    check_expr(texture->is_currently_mapped);

    xgl_unmap_texture_impl(texture->gpu_id);

    texture->is_currently_mapped = false;

error:
    return;
}

void* xgl_get_texture_handle(xgl_texture p_texture)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_texture *texture = object_pool_get(storage->textures, p_texture.handle);
    check_ptr(texture);

    return xgl_get_texture_handle_impl(texture->gpu_id);

error:
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////

result_e xgl_create_shader(struct xgl_shader_create_info* info, xgl_shader* p_shader)
{
    check_ptr(info);
    check_ptr(p_shader);

    check_ptr(info->vertex_shader_stage);
    check_ptr(info->fragment_shader_stage);

    if (!string_is_valid(info->name)) {
        info->name = make_string("<unnamed shader>");
    }

    ////////////////////////////////////////

    struct xgl_shader_stage_desc *vs = info->vertex_shader_stage;
    struct xgl_shader_stage_desc *fs = info->fragment_shader_stage;

    struct xgl_shader shader = {0};
    shader.name = info->name;

    shader.gpu_id = xgl_create_shader_impl(vs->src_ptr, fs->src_ptr);
    check_guid(shader.gpu_id);

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    p_shader->handle = object_pool_alloc(storage->shaders, &shader);
    check_guid(p_shader->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_destroy_shader(xgl_shader p_shader)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_shader *shader = object_pool_get(storage->shaders, p_shader.handle);
    check_ptr(shader);

    xgl_destroy_shader_impl(shader->gpu_id);

    object_pool_free(storage->shaders, p_shader.handle);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

static result_e _create_pipeline_shader_state(struct xgl_pipeline *pipeline, struct xgl_shader_state *shader_state)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    pipeline->shader_state = *shader_state;

    struct xgl_shader *shader = object_pool_get(storage->shaders, pipeline->shader_state.shader.handle);
    check_ptr(shader);

    result_e result = xgl_set_pipeline_shader_state_impl(pipeline->gpu_id, shader->gpu_id);
    check_result(result, "xgl_set_pipeline_shader_state_impl() failed");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_pipeline_input_state(struct xgl_pipeline *pipeline,
    struct xgl_ia_state *ia_state, struct xgl_input_layout *input_layout)
{
    pipeline->ia_state = *ia_state;

    if (input_layout)
    {
        check_ptr(input_layout->attrib_inputs);
        check_expr(input_layout->attrib_input_count > 0);

        ////////////////////////////////////////

        // vertex attrib inputs
        pipeline->attrib_inputs = vector_create(input_layout->attrib_input_count, sizeof(struct xgl_vertex_attrib_input));
        check_ptr(pipeline->attrib_inputs);

        for (u32 i = 0; i < input_layout->attrib_input_count; i++) {
            vector_push_back(pipeline->attrib_inputs, input_layout->attrib_inputs[i]);
        }
    }

    ////////////////////////////////////////

    result_e result = xgl_set_pipeline_input_state_impl(pipeline->gpu_id,
        pipeline->ia_state.topology, pipeline->attrib_inputs);
    check_result(result, "xgl_set_pipeline_input_state_impl() failed");

    return RC_SUCCESS;

error:

    if (pipeline->buffer_inputs) {
        vector_destroy(pipeline->buffer_inputs);
    }

    if (pipeline->attrib_inputs) {
        vector_destroy(pipeline->attrib_inputs);
    }

    return RC_FAILURE;
}

static result_e _create_pipeline_depth_stencil_state(struct xgl_pipeline *pipeline, struct xgl_depth_stencil_state *ds)
{
    if (ds) {
        pipeline->depth_stencil_state = *ds;
    }

    result_e result = xgl_set_pipeline_depth_state_impl(pipeline->gpu_id, &pipeline->depth_stencil_state.depth);
    check_result(result, "xgl_set_pipeline_depth_state_impl() failed");

    result = xgl_set_pipeline_stencil_state_impl(pipeline->gpu_id, &pipeline->depth_stencil_state.stencil);
    check_result(result, "xgl_set_pipeline_stencil_state_impl() failed");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_pipeline_rasterizer_state(struct xgl_pipeline *pipeline, struct xgl_rasterizer_state *rasterizer_state)
{
    if (rasterizer_state) {
        pipeline->rasterizer_state = *rasterizer_state;
    }

    if (pipeline->rasterizer_state.point_size == 0) {
        pipeline->rasterizer_state.point_size = 1.0f;
    }

    if (pipeline->rasterizer_state.line_width == 0) {
        pipeline->rasterizer_state.line_width = 1.0f;
    }

    result_e result = xgl_set_pipeline_rasterizer_state_impl(pipeline->gpu_id, &pipeline->rasterizer_state);
    check_result(result, "xgl_set_pipeline_rasterizer_state_impl() failed");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_pipeline_color_blend_state(struct xgl_pipeline *pipeline, struct xgl_color_blend_state *bs)
{
    if (bs) {
        pipeline->color_blend_state = *bs;
    }

    result_e result = xgl_set_pipeline_color_blend_state_impl(pipeline->gpu_id, &pipeline->color_blend_state);
    check_result(result, "xgl_set_pipeline_color_blend_state_impl() failed");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e xgl_create_pipeline(struct xgl_pipeline_create_info* info, xgl_pipeline* p_pipeline)
{
    check_ptr(info);
    check_ptr(p_pipeline);

    check_ptr(info->ia_state);
    check_ptr(info->shader_state);

    check_expr(info->type != XGL_PIPELINE_TYPE_UNKNOWN);
    check_expr(info->ia_state->topology != XGL_TOPOLOGY_UNKNOWN);

    if (!string_is_valid(info->name)) {
        info->name = make_string("<unnamed pipeline>");
    }

    ////////////////////////////////////////

    result_e result = RC_FAILURE;

    struct xgl_pipeline pipeline = {0};
    pipeline.name = info->name;
    pipeline.type = info->type;

    pipeline.gpu_id = xgl_create_pipeline_impl(pipeline.name, pipeline.type);
    check_guid(pipeline.gpu_id);

    result = _create_pipeline_shader_state(&pipeline, info->shader_state);
    check_result(result, "_create_pipeline_shader_state() failed");

    result = _create_pipeline_input_state(&pipeline, info->ia_state, info->input_layout);
    check_result(result, "_create_pipeline_input_state() failed");

    result = _create_pipeline_depth_stencil_state(&pipeline, info->depth_stencil_state);
    check_result(result, "_create_pipeline_depth_stencil_state() failed");

    result = _create_pipeline_rasterizer_state(&pipeline, info->rasterizer_state);
    check_result(result, "_create_pipeline_rasterizer_state() failed");

    result = _create_pipeline_color_blend_state(&pipeline, info->color_blend_state);
    check_result(result, "_create_pipeline_color_blend_state() failed");

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    p_pipeline->handle = object_pool_alloc(storage->pipelines, &pipeline);
    check_guid(p_pipeline->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_destroy_pipeline(xgl_pipeline p_pipeline)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline.handle);
    check_ptr(pipeline);

    xgl_destroy_pipeline_impl(pipeline->gpu_id);

    ////////////////////////////////////////

    if (pipeline->buffer_inputs) {
        vector_destroy(pipeline->buffer_inputs);
    }

    if (pipeline->attrib_inputs) {
        vector_destroy(pipeline->attrib_inputs);
    }

    ////////////////////////////////////////

    object_pool_free(storage->pipelines, p_pipeline.handle);

error:
    return;
}

result_e xgl_create_pipeline_layout(struct xgl_pipeline_layout_create_info* info, xgl_pipeline_layout* p_layout)
{
    check_ptr(info);
    check_ptr(p_layout);

    ////////////////////////////////////////

    struct xgl_pipeline_layout layout = {0};

    for (u32 i = 0; i < info->ds_layout_count; i++)
    {
        layout.ds_layouts[i] = info->ds_layouts[i];
        layout.ds_layout_count++;
    }

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    p_layout->handle = object_pool_alloc(storage->pipeline_layouts, &layout);
    check_guid(p_layout->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_destroy_pipeline_layout(xgl_pipeline_layout p_layout)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    object_pool_free(storage->pipeline_layouts, p_layout.handle);

error:
    return;
}

static u32 _hash_descriptor_set_layout(struct string name, struct xgl_descriptor_binding *bindings, u32 count)
{
    // FIXME ugly
    const u32 max_bindings = 16;
    const u32 max_binding_str_len = 12;

    u32 str_buf_len = 0;

    const u32 str_buf_max = max_bindings * max_binding_str_len;
    char str_buf[str_buf_max];

    for (u32 i = 0; i < count; i++)
    {
        check_expr(str_buf_max > (str_buf_len + max_binding_str_len));

        struct xgl_descriptor_binding *binding = &bindings[i];

        snprintf(str_buf + str_buf_len, max_binding_str_len + 1,
            "%.3d_%.3d_%.3d;", count, binding->binding, binding->type);

        str_buf_len = str_buf_len + max_binding_str_len;
    }

    u32 hash = hash_fnv1a_32(str_buf, str_buf_len);

    clog_trace("[ %.12u ] (%d bindings, %d/%d bytes) << %s ( %S )", hash, count, str_buf_len, str_buf_max, str_buf, &name);

    return hash;

error:
    return 0;
}

result_e xgl_create_descriptor_set_layout(struct xgl_descriptor_set_layout_create_info *info, xgl_descriptor_set_layout *p_layout)
{
    check_ptr(info);
    check_ptr(p_layout);

    check_ptr(info->bindings);
    check_expr(info->binding_count > 0);

    if (!string_is_valid(info->name)) {
        info->name = make_string("<unnamed ds layout>");
    }

    ////////////////////////////////////////

    struct xgl_descriptor_set_layout layout = {0};
    layout.format = _hash_descriptor_set_layout(info->name, info->bindings, info->binding_count);

    layout.bindings = vector_create(info->binding_count, sizeof(struct xgl_descriptor_binding));
    check_ptr(layout.bindings);

    // FIXME check binding point overlapping
    for (u32 i = 0; i < info->binding_count; i++)
    {
        struct xgl_descriptor_binding *binding = &info->bindings[i];

        switch (binding->type)
        {
            case XGL_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                layout.buffer_count++;
            break;

            case XGL_DESCRIPTOR_TYPE_TEXTURE_SAMPLER:
                layout.texture_count++;
            break;

            default:
                clog_trace("unknown binding type");
        }

        vector_set(layout.bindings, i, *binding);
    }

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    p_layout->handle = object_pool_alloc(storage->descriptor_set_layouts, &layout);
    check_guid(p_layout->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void xgl_destroy_descriptor_set_layout(xgl_descriptor_set_layout p_layout)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_descriptor_set_layout *layout = object_pool_get(storage->descriptor_set_layouts, p_layout.handle);
    check_ptr(layout);

    if (layout->bindings) {
        vector_destroy(layout->bindings);
    }

    object_pool_free(storage->descriptor_set_layouts, p_layout.handle);

error:
    return;
}

result_e xgl_create_descriptor_set(xgl_descriptor_set_layout p_layout, xgl_descriptor_set *p_set)
{
    check_ptr(p_set);

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_descriptor_set_layout *layout = object_pool_get(storage->descriptor_set_layouts, p_layout.handle);
    check_ptr(layout);

    struct xgl_descriptor_set set = {0};
    set.set_layout = p_layout;

    if (layout->buffer_count > 0) {
        set.buffer_descriptors = vector_create(layout->buffer_count, sizeof(struct xgl_buffer_descriptor));
        check_ptr(set.buffer_descriptors);
    }

    if (layout->texture_count > 0) {
        set.texture_descriptors = vector_create(layout->texture_count, sizeof(struct xgl_texture_descriptor));
        check_ptr(set.texture_descriptors);
    }

    ////////////////////////////////////////

    u32 layout_binding_count = vector_size(layout->bindings);

    for (u32 i = 0; i < layout_binding_count; i++)
    {
        struct xgl_descriptor_binding *binding = vector_get(layout->bindings, i);

        switch (binding->type)
        {
            case XGL_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            {
                check(layout->buffer_count > 0, "cannot add a buffer descriptor for non existing buffers");

                struct xgl_buffer_descriptor descriptor = {0};
                descriptor.binding = binding->binding;

                vector_push_back(set.buffer_descriptors, descriptor);
            }
            break;

            case XGL_DESCRIPTOR_TYPE_TEXTURE_SAMPLER:
            {
                check(layout->texture_count > 0, "cannot add a texture descriptor for non existing textures");

                struct xgl_texture_descriptor descriptor = {0};
                descriptor.binding = binding->binding;

                vector_push_back(set.texture_descriptors, descriptor);
            }
            break;

            default:
                clog_trace("descriptor binding type unknown");
        }
    }

    ////////////////////////////////////////

    p_set->handle = object_pool_alloc(storage->descriptor_sets, &set);
    check_guid(p_set->handle);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e xgl_update_descriptor_set(xgl_descriptor_set p_set, struct xgl_descriptor_set_update_info *update)
{
    check_ptr(update);

    if (update->buffer_descriptors) {
        check_expr(update->buffer_descriptor_count > 0);
    }

    if (update->texture_descriptors) {
        check_expr(update->texture_descriptor_count > 0);
    }

    ////////////////////////////////////////

    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_descriptor_set *set = object_pool_get(storage->descriptor_sets, p_set.handle);
    check_ptr(set);

    for (u32 i = 0; i < update->buffer_descriptor_count; i++)
    {
        struct xgl_buffer_descriptor *src_descriptor = &update->buffer_descriptors[i];

        for (u32 j = 0; j < vector_size(set->buffer_descriptors); j++)
        {
            struct xgl_buffer_descriptor *dst_descriptor = vector_get(set->buffer_descriptors, j);

            if (src_descriptor->binding == dst_descriptor->binding) {
                vector_set(set->buffer_descriptors, j, *src_descriptor);
            }
        }
    }

    for (u32 i = 0; i < update->texture_descriptor_count; i++)
    {
        struct xgl_texture_descriptor *src_descriptor = &update->texture_descriptors[i];

        for (u32 j = 0; j < vector_size(set->texture_descriptors); j++)
        {
            struct xgl_texture_descriptor *dst_descriptor = vector_get(set->texture_descriptors, j);

            if (src_descriptor->binding == dst_descriptor->binding) {
                vector_set(set->texture_descriptors, j, *src_descriptor);
            }
        }
    }

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;

}

void xgl_destroy_descriptor_set(xgl_descriptor_set p_set)
{
    struct xgl_storage *storage = xgl_storage_ptr();

    struct xgl_descriptor_set *set = object_pool_get(storage->descriptor_sets, p_set.handle);
    check_ptr(set);

    if (set->buffer_descriptors) {
        vector_destroy(set->buffer_descriptors);
    }

    if (set->texture_descriptors) {
        vector_destroy(set->texture_descriptors);
    }

    object_pool_free(storage->descriptor_sets, p_set.handle);

error:
    return;
}
