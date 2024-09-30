////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct material
{
    struct string name;

    struct material_effect {
        xgl_pipeline pipeline;
        xgl_pipeline_layout pipeline_layout;
    } effect;

    struct material_shader_data
    {
        struct{
            struct shader_data_material cpu;
            xgl_buffer gpu;
        } buffer;

        xgl_descriptor_set ds;
    } shader_data;
};
