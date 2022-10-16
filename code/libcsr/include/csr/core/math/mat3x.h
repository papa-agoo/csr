////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vec3.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mat33
{
    // column major format
    // xx yx zx
    // xy yy zy
    // xz yz zz

    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

#define make_mat33(...) \
    ((struct mat33){__VA_ARGS__})

////////////////////////////////////////////////////////////////////////////////

CSR_INLINE struct mat33 mat33_identity()
{
    //  1  0  0
    //  0  1  0
    //  0  0  1

    return make_mat33(.xx = 1.0f, .yy = 1.0f, .zz = 1.0f);
}

CSR_INLINE struct mat33 mat33_scale(struct vec3 s)
{
    // Sx  0  0  0
    //  0 Sy  0  0
    //  0  0 Sz  0
    //  0  0  0  1

    struct mat33 m = mat33_identity();

    m.xx = s.x;
    m.yy = s.y;
    m.zz = s.z;

    return m;
}
