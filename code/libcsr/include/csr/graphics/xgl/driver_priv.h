////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct xgl_buffer
{
    u32 usage_flags;

    u32 byte_length;

    bool is_currently_mapped;

    guid gpu_id;
};

struct xgl_sampler
{
    struct string name;

    // filter settings
    enum xgl_texture_filter min_filter;
    enum xgl_texture_filter mag_filter;

    // address settings
    enum xgl_sampler_addr_mode addr_mode_u;
    enum xgl_sampler_addr_mode addr_mode_v;
    enum xgl_sampler_addr_mode addr_mode_w;

    f32 border_color[4];

    // mipmap settings
    enum xgl_mipmap_mode mipmap_mode;
    f32 mip_lod_bias;
    f32 min_lod;
    f32 max_lod;

    // anisotropy settings (0 = disabled)
    f32 max_anisotropy;

    guid gpu_id;
};

struct xgl_texture
{
    enum xgl_texture_type type;
    enum xgl_texture_format format;

    u32 usage_flags;

    u32 width;
    u32 height;

    u32 sample_count;
    u32 mip_level_count;
    u32 array_layer_count;

    bool storage_allocated;
    bool is_currently_mapped;

    guid gpu_id;
};

struct xgl_shader
{
    struct string name;

    // struct xgl_shader_reflection_data reflection;

    guid gpu_id;
};

struct xgl_framebuffer
{
    u32 width;
    u32 height;

    u32 format; // FIXME

    // xgl_texture
    struct vector *attachments;

    guid gpu_id;
};

struct xgl_render_target
{
    xgl_texture color_buffer;
    xgl_texture depth_stencil_buffer;

    xgl_framebuffer framebuffer;
};

struct xgl_swapchain
{
    u32 width;
    u32 height;

    u32 rt_current_idx;
    struct xgl_render_target rt[2];
};

struct xgl_render_pass
{
    u32 format; // FIXME
};

struct xgl_descriptor_set_layout
{
    struct vector *bindings;

    u32 buffer_count;
    u32 texture_count;

    u32 format;
};

struct xgl_descriptor_set
{
    struct vector *buffer_descriptors;
    struct vector *texture_descriptors;

    xgl_descriptor_set_layout set_layout;
};

struct xgl_pipeline_layout
{
    xgl_descriptor_set_layout ds_layouts[XGL_DESCRIPTOR_SET_TYPE_MAX];
    u32 ds_layout_count;
};

struct xgl_pipeline
{
    struct string name;
    enum xgl_pipeline_type type;

    struct xgl_ia_state ia_state;
    struct xgl_shader_state shader_state;
    struct xgl_depth_stencil_state depth_stencil_state;
    struct xgl_rasterizer_state rasterizer_state;
    struct xgl_color_blend_state color_blend_state;

    struct vector *buffer_inputs; // FIXME move from xgl_pipeline to xgl_state
    struct vector *attrib_inputs;

    guid gpu_id;
};

////////////////////////////////////////////////////////////////////////////////

struct xgl_storage
{
    struct object_pool *buffers;
    struct object_pool *samplers;
    struct object_pool *textures;
    struct object_pool *shaders;
    struct object_pool *pipelines;
    struct object_pool *pipeline_layouts;
    struct object_pool *descriptor_set_layouts;
    struct object_pool *descriptor_sets;
    struct object_pool *framebuffers;
    struct object_pool *swapchains;
};

// struct xgl_device_info
// {
//     // gpu name
//     // gpu features
//     // vram info
// };

// struct xgl_device
// {
//     struct xgl_device_info info;

//     struct xgl_storage storage;
// };

struct xgl_state
{
    struct xgl_pipeline *pipeline;

    struct xgl_viewport viewport;
    struct xgl_rect scissor;
};

// struct xgl_command
// {
//     xgl_framebuffer framebuffer;

//     xgl_pipeline pipeline;

//     xgl_buffer index_buffer;

//     u32 vertex_buffer_count;
//     xgl_buffer* vertex_buffers;

//     u32 uniform_buffer_count;
//     xgl_buffer* uniform_buffers;

//     u32 texture_unit_count;
//     xgl_texture* texture_units;

//     struct xgl_viewport viewport;
//     struct xgl_rect scissor;
// };

struct xgl_driver
{
    bool is_initialized;

    struct xgl_state state;
    struct xgl_storage storage;

    struct xgl_driver_info info;
};

struct xgl_driver* xgl_driver_ptr();

#define xgl_state_ptr() (&xgl_driver_ptr()->state)
#define xgl_storage_ptr() (&xgl_driver_ptr()->storage)
