////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rgpu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rgpu_cache
{
    // depth stencil states
    struct {
        struct xgl_depth_stencil_state foo;
    } depth_stencil_state;

    // rasterizer states
    struct {
        struct xgl_rasterizer_state foo;
    } rasterizer_state;

    // color blend states
    struct {
        struct xgl_color_blend_state foo;
    } color_blend_state;

    // input layouts
    struct {
        struct xgl_input_layout position;
        struct xgl_input_layout position_color;
        struct xgl_input_layout position_texture;
    } input_layout;

    // descriptor set layouts
    struct {
        xgl_descriptor_set_layout frame;
        xgl_descriptor_set_layout pass;
        xgl_descriptor_set_layout material;
        xgl_descriptor_set_layout object;
    } ds_layout;

    // pipeline layouts
    struct {
        xgl_pipeline_layout main;
        xgl_pipeline_layout environment;
    } pipeline_layout;

    // shaders
    struct {
        xgl_shader vertex_color;
        xgl_shader vertex_texture;
    } shader;

    // pipelines
    struct {
        xgl_pipeline lines;
        xgl_pipeline lines_thick;
        xgl_pipeline lines_fat;

        xgl_pipeline debug_uvs;
        xgl_pipeline debug_normals;
    } pipeline;

    // samplers
    struct {
        xgl_sampler linear;
        xgl_sampler nearest;
        xgl_sampler anisotropic;
    } sampler;
};

struct rgpu
{
    struct xgl_viewport vp;

    struct rgpu_cache cache;
};

result_e rgpu_create_cache(struct rgpu_cache *cache);
void rgpu_destroy_cache(struct rgpu_cache *cache);
