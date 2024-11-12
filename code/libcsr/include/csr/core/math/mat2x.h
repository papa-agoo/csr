////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vec2.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mat22
{
    // column major format
    // xx yx
    // xy yy

    f32 xx, xy;
    f32 yx, yy;
};

#define make_mat22(...) \
    make_compound_literal(struct mat22, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////

CSR_INLINE struct mat22 mat22_identity()
{
    //  1  0
    //  0  1

    return make_mat22(.xx = 1.0f, .yy = 1.0f);
}

CSR_INLINE struct mat22 mat22_scale(struct vec2 s)
{
    // Sx  0
    //  0 Sy

    struct mat22 m = mat22_identity();

    m.xx = s.x;
    m.yy = s.y;

    return m;
}
