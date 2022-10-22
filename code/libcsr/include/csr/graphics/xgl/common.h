////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/vector.h>

#include <csr/core/math/rect.h>
#include <csr/core/math/vec4.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define XGL_OBJECT_DECLARE(_name) typedef struct { guid handle; } _name

enum xgl_driver_type
{
    XGL_DRIVER_TYPE_UNKNOWN,

    XGL_DRIVER_TYPE_OPENGL,
};

struct xgl_driver_features
{
    u64 debug_mode_enabled  : 1;
    u64 compute_pipeline    : 1;
    u64 geometry_shader     : 1;
    u64 tesselation_shader  : 1;
    u64 shader_reflection   : 1;
};

struct xgl_driver_info
{
    const char *name;

    struct version version;
    enum xgl_driver_type type;

    struct xgl_driver_features features;
};

////////////////////////////////////////////////////////////////////////////////

enum xgl_type
{
    XGL_TYPE_UNKNOWN,

    XGL_TYPE_U8,
    XGL_TYPE_S8,

    XGL_TYPE_U16,
    XGL_TYPE_S16,

    XGL_TYPE_U32,
    XGL_TYPE_S32,

    XGL_TYPE_F32,

    XGL_TYPE_F32_VEC2,
    XGL_TYPE_F32_VEC3,
    XGL_TYPE_F32_VEC4,

    XGL_TYPE_F32_MAT33,
    XGL_TYPE_F32_MAT44,

    XGL_TYPE_F64,
};

struct xgl_viewport
{
    f32 x, y;
    f32 width, height;
    f32 min_depth, max_depth;
};

struct xgl_rect
{
    s32 x, y;
    s32 width, height;
};

////////////////////////////////////////////////////////////////////////////////

XGL_OBJECT_DECLARE(xgl_buffer);

enum xgl_buffer_usage
{
    XGL_BUFFER_USAGE_UNKNOWN            = 0,

    // FIXME keep bitmap?
    XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT  = BIT(0),
    XGL_BUFFER_USAGE_INDEX_BUFFER_BIT   = BIT(1),
    XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT = BIT(2),
};

struct xgl_buffer_create_info
{
    u32 usage_flags;

    u32 byte_length;
    void *data;
};

////////////////////////////////////////////////////////////////////////////////

XGL_OBJECT_DECLARE(xgl_texture);

enum xgl_texture_filter
{
    XGL_TEXTURE_FILTER_LINEAR,
    XGL_TEXTURE_FILTER_NEAREST,
};

enum xgl_texture_type
{
    XGL_TEXTURE_TYPE_UNKNOWN,

    XGL_TEXTURE_TYPE_1D,
    XGL_TEXTURE_TYPE_1D_ARRAY,

    XGL_TEXTURE_TYPE_2D,
    XGL_TEXTURE_TYPE_2D_ARRAY,

    XGL_TEXTURE_TYPE_3D,

    XGL_TEXTURE_TYPE_CUBE_MAP,
    XGL_TEXTURE_TYPE_CUBE_MAP_ARRAY,
};

enum xgl_texture_format
{
    XGL_TEXTURE_FORMAT_UNKNOWN,

    XGL_TEXTURE_FORMAT_RGBA,
    XGL_TEXTURE_FORMAT_D24_S8,
};

enum xgl_texture_usage
{
    XGL_TEXTURE_USAGE_UNKNOWN                       = 0,

    // FIXME keep bitmap?
    XGL_TEXTURE_USAGE_SAMPLED_BIT                   = BIT(0),
    XGL_TEXTURE_USAGE_COLOR_ATTACHMENT_BIT          = BIT(1),
    XGL_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT  = BIT(2),
};

struct xgl_texture_create_info
{
    enum xgl_texture_type type;
    enum xgl_texture_format format;

    u32 usage_flags;

    u32 width;
    u32 height;

    u32 sample_count;
    u32 mip_level_count;
    u32 array_layer_count;
};

////////////////////////////////////////////////////////////////////////////////

XGL_OBJECT_DECLARE(xgl_sampler);

enum xgl_mipmap_mode
{
    XGL_MIPMAP_MODE_LINEAR,
    XGL_MIPMAP_MODE_NEAREST,
    XGL_MIPMAP_MODE_NONE,
};

enum xgl_sampler_addr_mode
{
    XGL_SAMPLER_ADDR_REPEAT,
    XGL_SAMPLER_ADDR_REPEAT_MIRRORED,
    XGL_SAMPLER_ADDR_CLAMP_TO_EDGE,
    XGL_SAMPLER_ADDR_CLAMP_TO_BORDER,
};

struct xgl_sampler_desc
{
    const char *name;

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
};

////////////////////////////////////////////////////////////////////////////////

XGL_OBJECT_DECLARE(xgl_shader);

enum xgl_shader_stage
{
    XGL_SHADER_STAGE_UNKNOWN,

    XGL_SHADER_STAGE_VERTEX,
    XGL_SHADER_STAGE_FRAGMENT,
};

struct xgl_shader_stage_desc
{
    enum xgl_shader_stage stage;

    void* src_ptr;
};

struct xgl_shader_reflection_data
{
    // FIXME
};

struct xgl_shader_create_info
{
    const char* name;

    struct xgl_shader_stage_desc *vertex_shader_stage;
    struct xgl_shader_stage_desc *fragment_shader_stage;
};

////////////////////////////////////////////////////////////////////////////////

XGL_OBJECT_DECLARE(xgl_framebuffer);

struct xgl_framebuffer_create_info
{
    u32 width;
    u32 height;

    xgl_texture *attachments;
    u32 attachment_count;
};

////////////////////////////////////////////////////////////////////////////////

XGL_OBJECT_DECLARE(xgl_swapchain);

struct xgl_swapchain_create_info
{
    u32 width;
    u32 height;
};

////////////////////////////////////////////////////////////////////////////////

XGL_OBJECT_DECLARE(xgl_render_pass);

struct xgl_clear_values
{
    struct vec4 color;
    f32 depth;
    s32 stencil;
};

struct xgl_render_pass_info
{
    const char* name;

    xgl_framebuffer framebuffer;
    struct xgl_clear_values clear_values;
};

////////////////////////////////////////////////////////////////////////////////

XGL_OBJECT_DECLARE(xgl_descriptor_set_layout);
XGL_OBJECT_DECLARE(xgl_descriptor_set);

XGL_OBJECT_DECLARE(xgl_pipeline_layout);
XGL_OBJECT_DECLARE(xgl_pipeline);

// ia state
enum xgl_topology
{
    XGL_TOPOLOGY_UNKNOWN,

    XGL_TOPOLOGY_POINT_LIST,

    XGL_TOPOLOGY_LINE_LIST,
    XGL_TOPOLOGY_LINE_STRIP,
    XGL_TOPOLOGY_LINE_LOOP,

    XGL_TOPOLOGY_TRIANGLE_LIST,
    XGL_TOPOLOGY_TRIANGLE_STRIP,
    XGL_TOPOLOGY_TRIANGLE_FAN,
};

struct xgl_ia_state
{
    enum xgl_topology topology;
};

// shader state
struct xgl_shader_state
{
    xgl_shader shader;
};

// depth / stencil state
enum xgl_compare_op
{
    XGL_COMPARE_OP_LESS,
    XGL_COMPARE_OP_GREATER,

    XGL_COMPARE_OP_EQUAL,
    XGL_COMPARE_OP_NOT_EQUAL,

    XGL_COMPARE_OP_LESS_EQUAL,
    XGL_COMPARE_OP_GREATER_EQUAL,

    XGL_COMPARE_OP_NEVER,
    XGL_COMPARE_OP_ALWAYS,
};

enum xgl_stencil_op
{
    XGL_STENCIL_OP_KEEP,
    XGL_STENCIL_OP_ZERO,
    XGL_STENCIL_OP_REPLACE,
    XGL_STENCIL_OP_INCREMENT_AND_CLAMP,
    XGL_STENCIL_OP_DECREMENT_AND_CLAMP,
    XGL_STENCIL_OP_INVERT,
    XGL_STENCIL_OP_INCREMENT_AND_WRAP,
    XGL_STENCIL_OP_DECREMENT_AND_WRAP,
};

struct xgl_stencil_op_state
{
    s32 ref_value;

    u32 write_mask; 
    u32 compare_mask;

    enum xgl_compare_op compare_op;

    enum xgl_stencil_op sfail_op;
    enum xgl_stencil_op spass_zfail_op;
    enum xgl_stencil_op spass_zpass_op;
};

struct xgl_depth_state
{
    bool enable_test;
    bool disable_write;

    enum xgl_compare_op compare_op;
};

struct xgl_stencil_state
{
    bool enable;

    struct xgl_stencil_op_state front;
    struct xgl_stencil_op_state back;
};

struct xgl_depth_stencil_state
{
    struct xgl_depth_state depth;
    struct xgl_stencil_state stencil;
};

// rasterizer state
enum xgl_cull_mode
{
    XGL_CULL_MODE_DISABLED,

    XGL_CULL_MODE_BACK,
    XGL_CULL_MODE_FRONT,
    XGL_CULL_MODE_FRONT_AND_BACK,
};

enum xgl_front_face
{
    XGL_FRONT_FACE_COUNTER_CLOCKWISE,
    XGL_FRONT_FACE_CLOCKWISE,
};

enum xgl_polygon_mode
{
    XGL_POLYGON_MODE_FILL,
    XGL_POLYGON_MODE_LINE,
    XGL_POLYGON_MODE_POINT,
};

struct xgl_rasterizer_state
{
    // FIXME depth clamping / bias
    // FIXME smooth line / polygon

    bool discard_primitives;

    // face culling
    enum xgl_cull_mode cull_mode;
    enum xgl_front_face front_face;

    // face rendering
    enum xgl_polygon_mode polygon_mode;

    f32 point_size;
    f32 line_width;
};

// color blend state
enum xgl_blend_factor
{
    XGL_BLEND_FACTOR_ZERO,
    XGL_BLEND_FACTOR_ONE,

    XGL_BLEND_FACTOR_SRC_COLOR,
    XGL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,

    XGL_BLEND_FACTOR_DST_COLOR,
    XGL_BLEND_FACTOR_ONE_MINUS_DST_COLOR,

    XGL_BLEND_FACTOR_SRC_ALPHA,
    XGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,

    XGL_BLEND_FACTOR_DST_ALPHA,
    XGL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,

    XGL_BLEND_FACTOR_CONSTANT_COLOR,
    XGL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,

    XGL_BLEND_FACTOR_CONSTANT_ALPHA,
    XGL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,

    XGL_BLEND_FACTOR_SRC_ALPHA_SATURATE,
};

enum xgl_blend_op
{
    XGL_BLEND_OP_ADD,
    XGL_BLEND_OP_SUBTRACT,
    XGL_BLEND_OP_REVERSE_SUBTRACT,
    XGL_BLEND_OP_MIN,
    XGL_BLEND_OP_MAX,
};

struct xgl_color_blend_state
{
    // FIXME dual source blending
    // FIXME color write masking
    // FIXME constant color / alpha

    bool enable;

    enum xgl_blend_op color_blend_op;
    enum xgl_blend_op alpha_blend_op;

    enum xgl_blend_factor src_color_blend_factor;
    enum xgl_blend_factor dst_color_blend_factor;

    enum xgl_blend_factor src_alpha_blend_factor;
    enum xgl_blend_factor dst_alpha_blend_factor;
};

// input layout state
enum xgl_vertex_attrib
{
    XGL_VERTEX_ATTRIB_POSITION      = 0,
    XGL_VERTEX_ATTRIB_COLOR         = 1,
    XGL_VERTEX_ATTRIB_NORMAL        = 2,
    XGL_VERTEX_ATTRIB_TEX_COORD0    = 3,
    XGL_VERTEX_ATTRIB_TEX_COORD1    = 4,
    XGL_VERTEX_ATTRIB_TANGENT       = 5,
    XGL_VERTEX_ATTRIB_BITANGENT     = 6,

    XGL_VERTEX_ATTRIB_MAX,
};

struct xgl_vertex_attrib_input
{
    u32 binding;
    u32 location;

    u32 offset;
};

struct xgl_input_layout
{
    u32 attrib_input_count;
    struct xgl_vertex_attrib_input *attrib_inputs;
};

// descriptor sets
enum xgl_descriptor_set_type
{
    XGL_DESCRIPTOR_SET_TYPE_PER_FRAME    = 0,
    XGL_DESCRIPTOR_SET_TYPE_PER_PASS     = 1,
    XGL_DESCRIPTOR_SET_TYPE_PER_MATERIAL = 2,
    XGL_DESCRIPTOR_SET_TYPE_PER_OBJECT   = 3,

    XGL_DESCRIPTOR_SET_TYPE_MAX
};

enum xgl_descriptor_type
{
    XGL_DESCRIPTOR_TYPE_UNKNOWN,

    XGL_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    XGL_DESCRIPTOR_TYPE_TEXTURE_SAMPLER,
};

struct xgl_descriptor_binding
{
    u32 binding;

    enum xgl_descriptor_type type;
};

struct xgl_buffer_descriptor
{
    u32 binding;

    xgl_buffer buffer;
};

struct xgl_texture_descriptor
{
    u32 binding;

    xgl_sampler sampler;
    xgl_texture texture;
};

struct xgl_descriptor_set_update_info
{
    struct xgl_buffer_descriptor *buffer_descriptors;
    u32 buffer_descriptor_count;

    struct xgl_texture_descriptor *texture_descriptors;
    u32 texture_descriptor_count;
};

struct xgl_descriptor_set_layout_create_info
{
    const char *name;

    struct xgl_descriptor_binding *bindings;
    u32 binding_count;
};

// pipelines
struct xgl_pipeline_layout_create_info
{
    xgl_descriptor_set_layout *ds_layouts;
    u32 ds_layout_count;
};

enum xgl_pipeline_type
{
    XGL_PIPELINE_TYPE_UNKNOWN,

    XGL_PIPELINE_TYPE_GRAPHICS,
    XGL_PIPELINE_TYPE_COMPUTE,
};

struct xgl_pipeline_create_info
{
    const char *name;
    enum xgl_pipeline_type type;

    struct xgl_ia_state *ia_state;
    struct xgl_shader_state *shader_state;
    struct xgl_rasterizer_state *rasterizer_state;
    struct xgl_depth_stencil_state *depth_stencil_state;
    struct xgl_color_blend_state *color_blend_state;

    struct xgl_input_layout *input_layout;
    xgl_pipeline_layout pipeline_layout;
    xgl_render_pass render_pass;
};
