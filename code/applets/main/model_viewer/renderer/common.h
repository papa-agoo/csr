////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/vector.h>

#include <csr/core/math.h>
#include <csr/core/math/aabb.h>
#include <csr/core/math/transform.h>

#include <csr/graphics/xgl.h>
#include <csr/graphics/softgl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////


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
// shader resources
////////////////////////////////////////////////////////////////////////////////////////////////////

// low frequency, updates once per frame
struct shader_data_frame
{
    struct mat44 mat_view;
    struct mat44 mat_projection;
    struct mat44 mat_projection_ortho;

    // ...
};

// medium frequency, updates once per render pass
struct shader_data_pass
{
    f32 foo;

    // ...
};

// average frequency, updates once per material
struct shader_data_material
{
    struct vec3 base_color;

    // ...
};

// high frequency, updates arbitrary
struct shader_data_object
{
    struct mat44 mat_model;
    struct mat44 mat_mvp;

    bool use_object_mvp;
    // ...
};
