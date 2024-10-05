////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/math.h>
#include <csr/graphics/pixelbuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define SOFTGL_OBJECT_DECLARE(_name) typedef struct { guid handle; } _name

struct softgl_driver_info
{
    struct string name;
    struct version version;
};

struct softgl_viewport
{
    f32 x, y;
    f32 width, height;
    f32 min_depth, max_depth;
};

struct softgl_buffer
{
    u32 byte_length;
    void* data;
};

struct softgl_index_buffer
{
    struct softgl_buffer buffer;
};

struct softgl_vertex_buffer
{
    struct softgl_buffer buffer;

    u32 stride;
};

// enum softgl_vertex_format
// {
//     SOFTGL_VERTEX_FORMAT_UNKNOWN = 0,

//     SOFTGL_VERTEX_FORMAT_1P,
//     SOFTGL_VERTEX_FORMAT_1P1N,
//     SOFTGL_VERTEX_FORMAT_1P1N1UV,
// };

struct softgl_shader_data
{
    struct softgl_shader_data_frame {
        struct mat44 view;
        struct mat44 projection;
    } frame;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// shaders
////////////////////////////////////////////////////////////////////////////////////////////////////

SOFTGL_OBJECT_DECLARE(softgl_shader);

enum softgl_vertex_attrib
{
    SOFTGL_VERTEX_ATTRIB_POSITION       = 0,
    SOFTGL_VERTEX_ATTRIB_NORMAL         = 1,
    SOFTGL_VERTEX_ATTRIB_COLOR          = 2,
    SOFTGL_VERTEX_ATTRIB_TEX_COORD0     = 3,
    SOFTGL_VERTEX_ATTRIB_TEX_COORD1     = 4,
    SOFTGL_VERTEX_ATTRIB_TANGENT        = 5,
    SOFTGL_VERTEX_ATTRIB_BITANGENT      = 6,

    SOFTGL_VERTEX_ATTRIB_MAX,
};

// vertex attrib inputs (provided by the geometry pipeline)
struct vertex_attrib_input_ptrs
{
    void* va_map[SOFTGL_VERTEX_ATTRIB_MAX];
};

// vertex attrib cache (written by vertex shader, read by fragment shader)
struct vertex_attrib_cache
{
    u32 count;
    struct vec4 slots[SOFTGL_VERTEX_ATTRIB_MAX];
};

struct softgl_vertex
{
    // system inputs
    u32 vertex_id;
    u32 primitive_id;

    // system outputs
    struct vec4 position;

    // user outputs (read by fragment shader)
    struct vertex_attrib_cache attribs_out;
};

struct softgl_fragment
{
    // system inputs
    struct vec4 frag_coords;
    bool front_facing;

    // system outputs
    struct vec4 color;

    // user inputs (written by vertex shader)
    struct vertex_attrib_cache attribs_in;
};

// shader stage callbacks
typedef void (softgl_vertex_shader_cb)(const struct vertex_attrib_input_ptrs* in, struct softgl_vertex* out);
typedef void (softgl_fragment_shader_cb)(struct softgl_fragment* io);

enum softgl_shader_type
{
    SOFTGL_SHADER_TYPE_UNKNOWN,

    SOFTGL_SHADER_TYPE_USER_FUNCTION,
    // SOFTGL_SHADER_TYPE_LUA_SCRIPT,
    // SOFTGL_SHADER_TYPE_BUILT_IN,
};

struct softgl_shader_create_info
{
    struct string name;

    softgl_vertex_shader_cb *vertex_shader_ptr;
    softgl_fragment_shader_cb *fragment_shader_ptr;

    enum softgl_shader_type type;
};

// vertex shader macros
#define vertex_shader(name) void name(const struct vertex_attrib_input_ptrs* in, struct softgl_vertex* out)

#define vs_in(semantic, type, var) const struct type* var = (struct type*) (in->va_map[SOFTGL_VERTEX_ATTRIB_##semantic])
#define vs_out(type, var) struct type* var = (struct type*) &(out->attribs_out.slots[out->attribs_out.count++])
#define vs_return(v) out->position = v

#define vs_vertex_id(var) const u32 var = out->vertex_id
#define vs_primitive_id(var) const u32 var = out->primitive_id

// fragment shader macros
#define fragment_shader(name) void name(struct softgl_fragment* io)

#define fs_in(slot, type, var) struct type* var = (struct type*) &(io->attribs_in.slots[slot])
#define fs_return(v) io->color = v

#define fs_frag_coords(var) const struct vec4* var = (&io->frag_coords)
#define fs_frag_depth(var) f32* var = (&io->frag_coords.z)
#define fs_front_facing(var) bool var = io->front_facing


////////////////////////////////////////////////////////////////////////////////

SOFTGL_OBJECT_DECLARE(softgl_pipeline);

enum softgl_topology
{
    SOFTGL_TOPOLOGY_UNKNOWN = 0,

    SOFTGL_TOPOLOGY_POINT_LIST,

    SOFTGL_TOPOLOGY_LINE_LIST,
    SOFTGL_TOPOLOGY_LINE_STRIP,
    SOFTGL_TOPOLOGY_LINE_LOOP,

    SOFTGL_TOPOLOGY_TRIANGLE_LIST,
    SOFTGL_TOPOLOGY_TRIANGLE_STRIP,
    SOFTGL_TOPOLOGY_TRIANGLE_FAN,
};

struct softgl_vertex_attrib_input
{
    enum softgl_vertex_attrib attrib;

    u32 binding;
    u32 offset;
};

struct softgl_input_layout
{
    u32 attrib_input_count;
    struct softgl_vertex_attrib_input *attrib_inputs;
};

struct softgl_ia_state
{
    enum softgl_topology topology;
};

struct softgl_shader_state
{
    softgl_shader shader;
};

struct softgl_pipeline_create_info
{
    struct string name;

    struct softgl_ia_state *ia_state;
    struct softgl_shader_state *shader_state;

    // struct softgl_rasterizer_state *rasterizer_state;
    // struct softgl_depth_stencil_state *depth_stencil_state;
    // struct softgl_color_blend_state *color_blend_state;

    struct softgl_input_layout *input_layout;
};
