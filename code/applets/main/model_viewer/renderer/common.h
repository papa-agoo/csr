////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/math.h>
#include <csr/core/math/aabb.h>
#include <csr/core/math/transform.h>

#include <csr/graphics/xgl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// low frequency, updates once per frame
struct shader_data_frame
{
    struct mat44 mat_view;
    struct mat44 mat_projection;
    struct mat44 mat_projection_ortho;
};

// medium frequency, updates once per render pass
struct shader_data_pass;

// average frequency, updates once per material
struct shader_data_material
{
    struct vec3 base_color;
};

// high frequency, updates arbitrary
struct shader_data_push
{
    struct mat44 mat_model;
};
