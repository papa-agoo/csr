////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e softgl_init_driver();
void softgl_quit_driver();

void softgl_get_driver_info(struct softgl_driver_info *info);


////////////////////////////////////////////////////////////////////////////////
// device api
////////////////////////////////////////////////////////////////////////////////
// result_e softgl_create_texture();
// void softgl_destroy_texture();

result_e softgl_create_shader(struct softgl_shader_create_info* info, softgl_shader* shader);
void softgl_destroy_shader(softgl_shader shader);

result_e softgl_create_pipeline(struct softgl_pipeline_create_info* info, softgl_pipeline* pipeline);
void softgl_destroy_pipeline(softgl_pipeline pipeline);


////////////////////////////////////////////////////////////////////////////////
// command api
////////////////////////////////////////////////////////////////////////////////
void softgl_set_viewport(struct softgl_viewport vp);
void softgl_set_pixelbuffer(struct pixelbuffer *pb);

void softgl_bind_pipeline(softgl_pipeline pipeline);
void softgl_bind_descriptor_set(enum softgl_descriptor_set_type type, const void* data);

void softgl_bind_index_buffer(struct softgl_index_buffer *buffer);
void softgl_bind_vertex_buffers(struct softgl_vertex_buffer **buffers, u32 buffer_count);

void softgl_draw(u32 first, u32 count);
void softgl_draw_indexed(u32 first, u32 count);
