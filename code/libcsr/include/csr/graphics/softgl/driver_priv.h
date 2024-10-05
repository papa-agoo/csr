////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// prevent "cache misses" for all atomic primitives (6 points, 3 lines or 2 triangles)
#define VERTEX_CACHE_SIZE 6 * 60

struct vertex_cache
{
    // index data (for vertex lookup)
    u32 index_count;
    u32 indices[VERTEX_CACHE_SIZE];

    // vertex shader output
    u32 vertex_count;
    struct softgl_vertex vertices[VERTEX_CACHE_SIZE];
};

struct primitive_cache
{
    u32 primitive_count;

    u32 vertex_count;
    struct softgl_vertex* vertices[VERTEX_CACHE_SIZE];
};

struct vertex_stream
{
    u32 head;
    u32 tail;

    u32 start;
    u32 stop;

    u32 seek_size;
};

struct input_assembler
{
    enum softgl_topology topology;

    struct vertex_stream vs;
    struct vertex_attrib_input_ptrs va_ptrs;

    u32 num_processed_vertices;
};

struct softgl_graphics_pipeline
{
    // input assembler
    struct input_assembler ia;

    // ia outputs
    struct vertex_cache vc;
    struct primitive_cache pc;
};

////////////////////////////////////////////////////////////

struct softgl_shader
{
    struct string name;

    softgl_vertex_shader_cb *vertex_shader_ptr;
    softgl_fragment_shader_cb *fragment_shader_ptr;

    enum softgl_shader_type type;
};

struct softgl_pipeline
{
    struct string name;

    enum softgl_topology topology;

    struct softgl_shader* shader;

    // ...

    struct vector *attrib_inputs; // FIXME?
};

struct softgl_storage
{
    struct object_pool *shaders;
    struct object_pool *pipelines;
};

#define SOFTGL_VERTEX_BUFFER_BINDING_COUNT 8

struct softgl_state
{
    struct pixelbuffer *pb;

    struct softgl_viewport viewport;
    struct softgl_shader_data shader_data;

    struct {
        struct softgl_pipeline *pipeline;
        struct softgl_index_buffer* index_buffer;
        struct softgl_vertex_buffer* vertex_buffers[SOFTGL_VERTEX_BUFFER_BINDING_COUNT];
        void* textures;
    } binding;
};

struct softgl_driver
{
    bool is_initialized;

    struct softgl_state state;
    struct softgl_storage storage;
    struct softgl_graphics_pipeline gp;

    struct softgl_driver_info info;
};

struct softgl_driver* softgl_driver_ptr();

#define softgl_state_ptr() (&softgl_driver_ptr()->state)
#define softgl_storage_ptr() (&softgl_driver_ptr()->storage)

#define softgl_binding_ptr() (softgl_state_ptr()->binding)
#define softgl_pso_ptr() (softgl_binding_ptr().pipeline)

#define softgl_gp_ptr() (&softgl_driver_ptr()->gp)
