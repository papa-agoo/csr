////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e xgl_init_driver();
void xgl_quit_driver();

void xgl_get_driver_info(struct xgl_driver_info *info);


////////////////////////////////////////////////////////////////////////////////
// device api
////////////////////////////////////////////////////////////////////////////////

// swapchain
result_e xgl_create_swapchain(struct xgl_swapchain_create_info *info, xgl_swapchain *swapchain);
void xgl_destroy_swapchain(xgl_swapchain swapchain);

void xgl_resize_swapchain(xgl_swapchain swapchain, u32 width, u32 height);

xgl_framebuffer xgl_get_swapchain_framebuffer(xgl_swapchain swapchain);
void xgl_present_swapchain_framebuffer(xgl_swapchain swapchain);


// framebuffers
result_e xgl_create_framebuffer(struct xgl_framebuffer_create_info* info, xgl_framebuffer* framebuffer);
void xgl_destroy_framebuffer(xgl_framebuffer framebuffer);


// buffers
result_e xgl_create_buffer(struct xgl_buffer_create_info* info, xgl_buffer* buffer);
void xgl_destroy_buffer(xgl_buffer buffer);
result_e xgl_update_buffer(xgl_buffer buffer, u32 offset, u32 byte_length, void *data);

void* xgl_map_buffer(xgl_buffer buffer);
void xgl_unmap_buffer(xgl_buffer buffer);


// samplers
result_e xgl_create_sampler(struct xgl_sampler_desc* desc, xgl_sampler* sampler);
void xgl_destroy_sampler(xgl_sampler sampler);


// textures
result_e xgl_create_texture(struct xgl_texture_create_info* info, xgl_texture* texture);
void xgl_destroy_texture(xgl_texture texture);
result_e xgl_update_texture(xgl_texture texture, u32 layer, u32 width, u32 height, u8 *data);

void* xgl_map_texture(xgl_texture texture);
void xgl_unmap_texture(xgl_texture texture);

void* xgl_get_texture_handle(xgl_texture texture);


// shaders
result_e xgl_create_shader(struct xgl_shader_create_info* info, xgl_shader* shader);
void xgl_destroy_shader(xgl_shader shader);


// pipelines
result_e xgl_create_pipeline(struct xgl_pipeline_create_info* info, xgl_pipeline* pipeline);
void xgl_destroy_pipeline(xgl_pipeline pipeline);

result_e xgl_create_pipeline_layout(struct xgl_pipeline_layout_create_info* info, xgl_pipeline_layout* layout);
void xgl_destroy_pipeline_layout(xgl_pipeline_layout layout);

result_e xgl_create_descriptor_set_layout(struct xgl_descriptor_set_layout_create_info *info, xgl_descriptor_set_layout *layout);
void xgl_destroy_descriptor_set_layout(xgl_descriptor_set_layout layout);

result_e xgl_create_descriptor_set(xgl_descriptor_set_layout layout, xgl_descriptor_set *set);
void xgl_destroy_descriptor_set(xgl_descriptor_set set);
result_e xgl_update_descriptor_set(xgl_descriptor_set set, struct xgl_descriptor_set_update_info *update);


////////////////////////////////////////////////////////////////////////////////
// command api
////////////////////////////////////////////////////////////////////////////////

// render pass commands
bool xgl_begin_render_pass(struct xgl_render_pass_info* info);
void xgl_end_render_pass();

void xgl_set_viewports(u32 count, struct xgl_viewport* viewports);
void xgl_set_scissor_rects(u32 count, struct xgl_rect* scissors);


// bind commands
void xgl_bind_pipeline(enum xgl_pipeline_type type, xgl_pipeline pipeline);
void xgl_bind_descriptor_set(enum xgl_descriptor_set_type type, xgl_pipeline_layout pipeline_layout, xgl_descriptor_set set);

void xgl_bind_index_buffer(xgl_buffer buffer);
void xgl_bind_vertex_buffers(u32 start_at, u32 count, xgl_buffer* buffers, u32* offsets, u32* strides);


// draw commands
void xgl_draw(u32 first, u32 count);
void xgl_draw_indexed(u32 first, u32 count);
