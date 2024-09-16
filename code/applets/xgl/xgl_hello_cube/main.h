////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/applet.h>
#include <csr/core/math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct cube_data
{
    struct mat44 mvp;
    f32 texture_scale;
};

struct cube_geometry
{
    struct {
        xgl_buffer positions;
        xgl_buffer tex_coords;
    } vertex_buffer;

    struct {
        xgl_buffer indices;
        u32 index_count;
    } index_buffer;
};

struct cube_material
{
    struct {
        xgl_shader shader;
        xgl_pipeline pipeline;
        xgl_pipeline_layout pipeline_layout;
    } effect;

    struct {
        struct cube_data cpu;
        xgl_buffer gpu;
    } shader_params;

    xgl_texture texture;
    xgl_sampler sampler;
};

struct my_cube
{
    xgl_descriptor_set descriptor_set;
    xgl_descriptor_set_layout descriptor_set_layout;
    
    struct cube_geometry geometry;
    struct cube_material material;
};

struct texture_info
{
    string_cstr filename;
    struct pixelbuffer* image;

    xgl_texture texture;
};

////////////////////////////////////////////////////////////////////////////////

void register_ui();

result_e create_resources();
void destroy_resources();
