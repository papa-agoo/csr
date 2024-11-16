////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "error_handling.h"

#include <csr/core/object_pool.h>
#include <csr/graphics/xgl/driver_pimpl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct gl_framebuffer
{
    u32 width;
    u32 height;

    GLuint id;
};

struct gl_sampler
{
    GLuint id;
};

#define GL_PBO_COUNT 3

struct gl_texture
{
    GLuint id;

    GLenum type;
    GLenum attachment_type;

    // https://afrantzis.com/pixel-format-guide/opengl.html
    GLenum pixel_format;
    GLenum pixel_size_type;
    GLenum storage_format;

    u32 width;
    u32 height;

    u32 mip_count;
    u32 sample_count;
    u32 layer_count;

    struct {
        u32 idx_prev;
        u32 idx_current;
        u32 idx_next;

        GLuint buffer[GL_PBO_COUNT];
        void *ptr;

        GLsizei byte_length;
        GLbitfield mapping_flags;
        GLbitfield storage_flags;
    } pbo;
};

struct gl_buffer
{
    GLuint id;

    GLenum type;
    GLsizei byte_length;

    GLbitfield mapping_flags;
    GLbitfield storage_flags;
};

struct gl_shader
{
    GLuint id;
};

struct gl_depth_state
{
    bool enable_test;
    bool enable_write;

    GLenum compare_op;
};

struct gl_stencil_op_state
{
    s32 ref_value;

    u32 write_mask;
    u32 compare_mask;

    GLenum compare_op;

    GLenum sfail_op;
    GLenum spass_zfail_op;
    GLenum spass_zpass_op;
};

struct gl_stencil_state
{
    bool enable;

    struct gl_stencil_op_state front;
    struct gl_stencil_op_state back;
};

struct gl_color_blend_state
{
    bool enable;

    GLenum color_blend_op;
    GLenum alpha_blend_op;

    GLenum src_color_blend_factor;
    GLenum src_alpha_blend_factor;

    GLenum dst_color_blend_factor;
    GLenum dst_alpha_blend_factor;
};

struct gl_rasterizer_state
{
    bool discard_primitives;

    bool enable_culling;
    GLenum front_face;
    GLenum cull_mode;

    GLenum polygon_mode;

    // bool enable_polygon_offsets;

    // f32 polygon_offset_factor;
    // f32 polygon_offset_units;

    f32 point_size;
    bool smooth_points;

    f32 line_width;
    bool smooth_lines;
};

struct gl_pipeline
{
    struct string name;

    GLenum topology;
    GLuint vertex_arrays;
    GLuint shader_program;

    struct gl_depth_state depth_state;
    struct gl_stencil_state stencil_state;
    struct gl_color_blend_state color_blend_state;
    struct gl_rasterizer_state rasterizer_state;
};

////////////////////////////////////////////////////////////////////////////////

struct gl_state
{
    GLuint *ebo; // FIXME why not opaque type?

    struct gl_pipeline *pso;
};

struct gl_storage
{
    struct object_pool *framebuffers;
    struct object_pool *textures;
    struct object_pool *samplers;
    struct object_pool *buffers;
    struct object_pool *shaders;
    struct object_pool *pipelines;
};

struct gl_driver
{
    bool is_initialized;

    struct gl_state state;
    struct gl_storage storage;
};

struct gl_driver* gl_driver_ptr();

#define gl_state_ptr() (&gl_driver_ptr()->state)
#define gl_storage_ptr() (&gl_driver_ptr()->storage)
