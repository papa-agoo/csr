////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e xgl_init_driver_impl(struct xgl_driver_info *info);
void xgl_quit_driver_impl();


////////////////////////////////////////////////////////////////////////////////
// device api
////////////////////////////////////////////////////////////////////////////////

// framebuffers
guid xgl_create_framebuffer_impl(struct vector *attachments);
void xgl_destroy_framebuffer_impl(guid p_framebuffer);
void xgl_present_framebuffer_impl(guid p_framebuffer);


// buffers
guid xgl_create_buffer_impl(u32 usage_flags, size_t byte_length, void *data);
void xgl_destroy_buffer_impl(guid p_buffer);
result_e xgl_update_buffer_impl(guid p_buffer, u32 offset, u32 byte_length, void *data);

void* xgl_map_buffer_impl(guid p_buffer, u32 offset, u32 byte_length);
void xgl_unmap_buffer_impl(guid p_buffer);

// samplers
guid xgl_create_sampler_impl(struct xgl_sampler_desc* desc);
void xgl_destroy_sampler_impl(guid p_sampler);


// textures
guid xgl_create_texture_impl(enum xgl_texture_type type, u32 sample_count, u32 usage_flags);
void xgl_destroy_texture_impl(guid p_texture);

result_e xgl_alloc_texture_storage_impl(guid p_texture, enum xgl_texture_format format, u32 mip_count, u32 width, u32 height, u32 layer_count);
result_e xgl_update_texture_impl(guid p_texture, u32 layer, u32 width, u32 height, u8 *data);
result_e xgl_generate_mipmaps_impl(guid p_texture);
void* xgl_get_texture_handle_impl(guid p_texture);


// shaders
guid xgl_create_shader_impl(void* vs_ptr, void* fs_ptr);
void xgl_destroy_shader_impl(guid p_shader);


// pipelines
guid xgl_create_pipeline_impl(const char *name, enum xgl_pipeline_type type);
void xgl_destroy_pipeline_impl(guid p_pipeline);

result_e xgl_set_pipeline_shader_state_impl(guid p_pipeline, guid p_shader);
result_e xgl_set_pipeline_input_state_impl(guid p_pipeline, enum xgl_topology topology, struct vector *attrib_inputs);
result_e xgl_set_pipeline_depth_state_impl(guid p_pipeline, struct xgl_depth_state *ds);
result_e xgl_set_pipeline_stencil_state_impl(guid p_pipeline, struct xgl_stencil_state *ss);
result_e xgl_set_pipeline_rasterizer_state_impl(guid p_pipeline, struct xgl_rasterizer_state *rs);
result_e xgl_set_pipeline_color_blend_state_impl(guid p_pipeline, struct xgl_color_blend_state *bs);


////////////////////////////////////////////////////////////////////////////////
// command api
////////////////////////////////////////////////////////////////////////////////

// global state
void xgl_reset_pipeline_state_impl();
void xgl_reset_resource_bindings_impl();


// render pass calls
void xgl_clear_color_buffer_impl(guid fb_id, u32 buffer_idx, f32 *rgba);
void xgl_clear_depth_stencil_buffer_impl(guid fb_id, f32 depth, u32 stencil);
void xgl_set_viewport_impl(struct xgl_viewport vp);
void xgl_set_scissor_rect_impl(struct xgl_rect sr);


// bind calls
void xgl_bind_pipeline_impl(guid pipeline);
void xgl_bind_framebuffer_impl(guid framebuffer);
void xgl_bind_index_buffer_impl(guid buffer);
void xgl_bind_vertex_buffer_impl(guid buffer, u32 binding, u32 offset, u32 stride);
void xgl_bind_uniform_buffer_impl(guid buffer, u32 binding);
void xgl_bind_texture_impl(guid texture, u32 texture_unit);
void xgl_bind_sampler_impl(guid sampler, u32 texture_unit);


// draw calls
void xgl_draw_impl(u32 first, u32 count);
void xgl_draw_indexed_impl(u32 count);
