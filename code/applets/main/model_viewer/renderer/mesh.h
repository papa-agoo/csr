////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

#include <csr/core/vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct material;

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

struct mesh_buffer
{
    u32 vertex_format;
    u32 vertex_stride;

    struct {
        struct vector *vertices;
        struct vector *indices;
    } cpu;

    struct {
        xgl_buffer vertices;
        xgl_buffer indices;
    } gpu;
};

struct mesh
{
    struct vector *primitives;

    struct {
        struct shader_data_push cpu;
        xgl_buffer gpu;
    } push_data;
};

struct mesh_primitive
{
    struct {
        u32 start;
        u32 count;
    } vertices;

    struct {
        u32 start;
        u32 count;
    } indices;

    struct material *material;
    struct mesh_buffer *buffer;
    // u32 buffer_id;
    // u32 material_id;
};

struct mesh_node
{
    struct aabb aabb;

    struct mesh *mesh;
    // u32 mesh_id;

    struct transform transform;

    struct mesh_node *parent;
    // children ...
};
