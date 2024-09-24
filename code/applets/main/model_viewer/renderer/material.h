////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct material_params
{
    bool is_dirty;

    struct shader_data_material cpu;
    xgl_buffer gpu;
};

struct material
{
    struct string name;

    struct material_effect {
        xgl_pipeline pipeline;
        xgl_pipeline_layout pipeline_layout;
    } effect;

    struct material_bindings {
        struct material_params params;
        // ...
    } bindings;
};
