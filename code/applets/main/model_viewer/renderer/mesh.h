////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct material;

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

    struct mesh_shader_data {
        struct {
            struct shader_data_object cpu;
            xgl_buffer gpu;
        } buffer;

        xgl_descriptor_set ds;
    } shader_data;
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
};

struct mesh_node
{
    struct aabb aabb;

    struct mesh *mesh;

    struct transform transform;

    struct mesh_node *parent;
    // children ...
};
