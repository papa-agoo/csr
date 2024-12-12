////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/vector.h>

#include <csr/core/math.h>
#include <csr/core/math/aabb.h>
#include <csr/core/math/transform.h>

#include <csr/graphics/xgl.h>
#include <csr/graphics/softgl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define RSX_OBJECT_DECLARE CSR_OBJECT_DECLARE

enum primitive_size
{
    PRIMITIVE_SIZE_NORMAL = 0,
    PRIMITIVE_SIZE_THICK,
    PRIMITIVE_SIZE_FAT,

    PRIMITIVE_SIZE_MAX,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// vertices
////////////////////////////////////////////////////////////////////////////////////////////////////
enum vertex_format
{
    VERTEX_FORMAT_POSITION_BIT      = BIT(0),
    VERTEX_FORMAT_COLOR_BIT         = BIT(1),
    VERTEX_FORMAT_NORMAL_BIT        = BIT(2),
    VERTEX_FORMAT_TEX_COORD0_BIT    = BIT(3),
    VERTEX_FORMAT_TEX_COORD1_BIT    = BIT(4),
    VERTEX_FORMAT_TANGENT_BIT       = BIT(5),
    VERTEX_FORMAT_BITANGENT_BIT     = BIT(6),

    VERTEX_FORMAT_1P                = VERTEX_FORMAT_POSITION_BIT,
    VERTEX_FORMAT_1P_1C             = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT,
    VERTEX_FORMAT_1P_1N             = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_NORMAL_BIT,
    VERTEX_FORMAT_1P_1N_1UV         = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_NORMAL_BIT | VERTEX_FORMAT_TEX_COORD0_BIT,
};

CSR_INLINE u32 vertex_format_get_stride(u32 vertex_format)
{
    u32 stride = 0;

    stride += (vertex_format & VERTEX_FORMAT_POSITION_BIT)      ? sizeof(struct vec3) : 0;
    stride += (vertex_format & VERTEX_FORMAT_COLOR_BIT)         ? sizeof(struct vec3) : 0;
    stride += (vertex_format & VERTEX_FORMAT_NORMAL_BIT)        ? sizeof(struct vec3) : 0;
    stride += (vertex_format & VERTEX_FORMAT_TEX_COORD0_BIT)    ? sizeof(struct vec2) : 0;
    stride += (vertex_format & VERTEX_FORMAT_TEX_COORD1_BIT)    ? sizeof(struct vec2) : 0;
    stride += (vertex_format & VERTEX_FORMAT_TANGENT_BIT)       ? sizeof(struct vec3) : 0;
    stride += (vertex_format & VERTEX_FORMAT_BITANGENT_BIT)     ? sizeof(struct vec3) : 0;

    return stride;
}

struct vertex_1p
{
    struct vec3 position;
};

struct vertex_1p1c
{
    struct vec3 position;
    struct vec3 color;
};

struct vertex_1p1uv
{
    struct vec3 position;
    struct vec2 texture;
};

struct vertex_1p1n1uv
{
    struct vec3 position;
    struct vec3 normal;
    struct vec2 texture;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// shader data
////////////////////////////////////////////////////////////////////////////////////////////////////

// low frequency, updates once per frame
struct shader_data_frame
{
    struct mat44 mtx_view;
    struct mat44 mtx_projection;
    struct mat44 mtx_projection_ortho;

    // ...
};

CSR_INLINE void shader_data_frame_init(struct shader_data_frame *data)
{
    data->mtx_view = mat44_identity();
    data->mtx_projection = mat44_identity();
    data->mtx_projection_ortho = mat44_identity();
}

// medium frequency, updates once per render pass
struct shader_data_pass_main
{
    f32 foo;

    // ...
};

CSR_INLINE void shader_data_pass_main_init(struct shader_data_pass_main *data)
{
    // ...
}

// average frequency, updates once per material
struct shader_data_material
{
    struct vec3 base_color;

    // ...
};

CSR_INLINE void shader_data_material_init(struct shader_data_material *data)
{
    // ...
}

// high frequency, updates arbitrary
struct shader_data_object
{
    struct mat44 mtx_model;
    struct mat44 mtx_mvp;

    bool use_object_mvp;
    // ...
};

CSR_INLINE void shader_data_object_init(struct shader_data_object *data)
{
    data->mtx_model = mat44_identity();
    data->mtx_mvp = mat44_identity();

    data->use_object_mvp = false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// shader resources
////////////////////////////////////////////////////////////////////////////////////////////////////
struct rsx_shader_resource_binding
{
    xgl_descriptor_set ds;
    xgl_descriptor_set_layout ds_layout;

    xgl_pipeline_layout pipeline_layout;
};

#define RSX_TEXTURE(BASE_TYPE, TEX_TYPE) // ...

#define RSX_UNIFORM_BUFFER(BASE_TYPE, UBO_TYPE) struct UBO_TYPE \
{ \
    struct BASE_TYPE cpu; \
    xgl_buffer gpu; \
}

#define RSX_SHADER_RESOURCE(BASE_TYPE, RESOURCE_TYPE) struct RESOURCE_TYPE \
{ \
    struct BASE_TYPE data; \
    struct rsx_shader_resource_binding binding; \
}

// declare shader resource types
RSX_UNIFORM_BUFFER(shader_data_frame, rsx_uniform_buffer_frame);
RSX_SHADER_RESOURCE(rsx_uniform_buffer_frame, rsx_shader_resource_frame);

RSX_UNIFORM_BUFFER(shader_data_pass_main, rsx_uniform_buffer_pass_main);
RSX_SHADER_RESOURCE(rsx_uniform_buffer_pass_main, rsx_shader_resource_pass_main);

RSX_UNIFORM_BUFFER(shader_data_material, rsx_uniform_buffer_material);
RSX_SHADER_RESOURCE(rsx_uniform_buffer_material, rsx_shader_resource_material);

RSX_UNIFORM_BUFFER(shader_data_object, rsx_uniform_buffer_object);
RSX_SHADER_RESOURCE(rsx_uniform_buffer_object, rsx_shader_resource_object);


////////////////////////////////////////////////////////////////////////////////////////////////////
// materials
////////////////////////////////////////////////////////////////////////////////////////////////////
struct rsx_material
{
    struct string name;
};

struct rsx_material_create_info
{
    struct string name;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// meshes
////////////////////////////////////////////////////////////////////////////////////////////////////
struct rsx_mesh
{
    struct string name;

    struct rsx_shader_resource_object shader_data;
};

struct rsx_mesh_create_info
{
    struct string name;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass + render data
////////////////////////////////////////////////////////////////////////////////////////////////////
struct rsx_pass_base
{
    struct string name;

    bool enabled;
};

struct rsx_pass_meshes
{
    struct rsx_pass_base base;

    // ...
};

struct rsx_pass_gizmos
{
    struct rsx_pass_base base;

    bool draw_grid;
    bool draw_orientation_axes;
    bool draw_transform_handles;

    // ...
};

struct rsx_pass_environment
{
    struct rsx_pass_base base;

    f32 blur_amount;
    bool blur_background;

    // ...
};

struct rsx_pass_debug_primitives
{
    struct rsx_pass_base base;

    // ...
};

struct rsx_render_data
{
    struct rsx_shader_resource_frame frame;

    // ...

    struct {
        struct rsx_pass_meshes meshes;
        struct rsx_pass_gizmos gizmos;
        struct rsx_pass_environment environment;
        struct rsx_pass_debug_primitives debug_primitives;

        // ...
    } pass;
};
