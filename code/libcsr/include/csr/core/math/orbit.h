////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vec3.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct orbit
{
    struct vec3 origin;

    f32 polar;
    f32 azimuth;
    f32 radius;
};

CSR_INLINE struct vec3 orbit_get_coords(struct orbit *orbit)
{
    check_ptr(orbit);
    check_expr(orbit->radius > 0);

    f32 p = DEG2RAD(orbit->polar);
    f32 a = DEG2RAD(orbit->azimuth);

    return make_vec3(
        .x = orbit->origin.x + orbit->radius * cosf(p) * cosf(a),
        .y = orbit->origin.y + orbit->radius * sinf(p),
        .z = orbit->origin.z + orbit->radius * cosf(p) * sinf(a)
    );

error:
    return make_vec3_z_axis();
}
