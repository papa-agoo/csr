////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// void gp_reset_state();

void gp_process_vertices(struct softgl_index_buffer *index_buffer);
void gp_process_primitives();

// input assembler
void gp_ia_setup(struct input_assembler* ia, enum softgl_topology topology, u32 first, u32 vertex_count);
bool gp_ia_seek_stream(struct vertex_stream* vs);

// clipper
u32 gp_clip_point(struct softgl_vertex* a);
u32 gp_clip_line(struct softgl_vertex* a, struct softgl_vertex* b);
u32 gp_clip_triangle(struct softgl_vertex* a, struct softgl_vertex* b, struct softgl_vertex* c);

// rasterizer
void gp_rasterize_point(struct softgl_vertex* a);
void gp_rasterize_line(struct softgl_vertex* a, struct softgl_vertex* b);
void gp_rasterize_triangle(struct softgl_vertex* a, struct softgl_vertex* b, struct softgl_vertex* c);
