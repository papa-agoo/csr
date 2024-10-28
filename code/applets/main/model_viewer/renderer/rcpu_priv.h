////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rcpu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rcpu_cache
{
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
        softgl_pipeline lines;
    } pipeline;
};

struct rcpu
{
    struct softgl_viewport vp;

    struct rcpu_cache cache;
};

result_e rcpu_create_cache(struct rcpu_cache *cache);
void rcpu_destroy_cache(struct rcpu_cache *cache);
