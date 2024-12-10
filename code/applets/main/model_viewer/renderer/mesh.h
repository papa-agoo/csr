////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct material;

struct mesh_shader_data {
    struct {
        struct shader_data_object cpu;
        xgl_buffer gpu;
    } buffer;

    xgl_descriptor_set ds;
};

struct mesh_buffer
{
    struct {
        struct vector *cpu;
        xgl_buffer gpu;
    } vertices;

    struct {
        struct vector *cpu;
        xgl_buffer gpu;
    } indices;
};

struct mesh_primitive
{
    u32 vertex_format;
    u32 vertex_stride;

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

struct mesh
{
    struct string name;

    struct vector *primitives; // pool?

    struct mesh_buffer *buffer;
    struct mesh_shader_data shader_data;

    struct aabb aabb;
};

struct mesh_node
{
    struct aabb aabb;

    struct mesh *mesh;

    struct transform transform;

    struct mesh_node *parent;
    // children ...
};
