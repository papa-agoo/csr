////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rcpu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rcpu_cache
{
    // depth stencil states
    struct {
        struct softgl_depth_stencil_state off_off;
        struct softgl_depth_stencil_state rw_off;
    } depth_stencil_state;

    // input layouts
    struct {
        struct softgl_input_layout position;
        struct softgl_input_layout position_color;
    } input_layout;

    // shaders
    struct {
        softgl_shader vertex_color;
    } shader;

    // pipelines
    struct {
        softgl_pipeline points;
        softgl_pipeline points_no_depth;

        softgl_pipeline lines;
        softgl_pipeline lines_no_depth;
    } pipeline;
};

struct rcpu
{
    struct softgl_viewport vp;

    struct rcpu_cache cache;
};

result_e rcpu_create_cache(struct rcpu_cache *cache);
void rcpu_destroy_cache(struct rcpu_cache *cache);
