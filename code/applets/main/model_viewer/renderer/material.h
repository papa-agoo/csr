////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

enum primitive_size
{
    PRIMITIVE_SIZE_NORMAL = 0,
    PRIMITIVE_SIZE_THICK,
    PRIMITIVE_SIZE_FAT,

    PRIMITIVE_SIZE_MAX,
};

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
