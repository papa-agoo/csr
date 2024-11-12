////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vec3.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

enum orbit_orientation
{
    ORBIT_ORIENTATION_NONE = 0,

    ORBIT_ORIENTATION_TOP,
    ORBIT_ORIENTATION_BOTTOM,

    ORBIT_ORIENTATION_FRONT,
    ORBIT_ORIENTATION_REAR,

    ORBIT_ORIENTATION_LEFT,
    ORBIT_ORIENTATION_RIGHT,

    ORBIT_ORIENTATION_LEFT_SIDE,
    ORBIT_ORIENTATION_RIGHT_SIDE,
};

struct orbit
{
    enum orbit_orientation orientation;

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

CSR_INLINE void orbit_set_polar(struct orbit *orbit, f32 polar)
{
    check_ptr(orbit);
    check_quiet(orbit->polar != polar);

    orbit->polar = polar;
    orbit->orientation = ORBIT_ORIENTATION_NONE;

error:
    return;
}

CSR_INLINE void orbit_set_azimuth(struct orbit *orbit, f32 azimuth)
{
    check_ptr(orbit);
    check_quiet(orbit->azimuth != azimuth);

    orbit->azimuth = clamp(azimuth, -360, 360);
    orbit->orientation = ORBIT_ORIENTATION_NONE;

error:
    return;
}

CSR_INLINE void orbit_set_radius(struct orbit *orbit, f32 radius)
{
    check_ptr(orbit);
    check_quiet(orbit->radius != radius);

    orbit->radius = max(radius, FLT_EPSILON);
    orbit->orientation = ORBIT_ORIENTATION_NONE;

error:
    return;
}

CSR_INLINE void orbit_set(struct orbit *orbit, struct vec3 origin, f32 polar, f32 azimuth, f32 radius)
{
    check_ptr(orbit);

    orbit->origin = origin;

    orbit_set_polar(orbit, polar);
    orbit_set_azimuth(orbit, azimuth);
    orbit_set_radius(orbit, radius);

error:
    return;
}

CSR_INLINE void orbit_set_orientation(struct orbit *orbit, enum orbit_orientation orientation)
{
    check_ptr(orbit);
    check_quiet(orientation != ORBIT_ORIENTATION_NONE);

    orbit->orientation = orientation;

    switch (orientation)
    {
        case ORBIT_ORIENTATION_TOP: {
            orbit->azimuth = 90;
            orbit->polar = 90 - FLT_EPSILON;
        }
        break;

        case ORBIT_ORIENTATION_BOTTOM: {
            orbit->azimuth = 90;
            orbit->polar = -90 - FLT_EPSILON;
        }
        break;

        case ORBIT_ORIENTATION_FRONT: {
            orbit->azimuth = 90;
            orbit->polar = 0;
        }
        break;

        case ORBIT_ORIENTATION_REAR: {
            orbit->azimuth = 270;
            orbit->polar = 0;
        }
        break;

        case ORBIT_ORIENTATION_LEFT: {
            orbit->azimuth = 180;
            orbit->polar = 0;
        }
        break;

        case ORBIT_ORIENTATION_RIGHT: {
            orbit->azimuth = 0;
            orbit->polar = 0;
        }
        break;

        case ORBIT_ORIENTATION_LEFT_SIDE: {
            orbit->azimuth = 135;
            orbit->polar = 15;
        }
        break;

        case ORBIT_ORIENTATION_RIGHT_SIDE: {
            orbit->azimuth = 45;
            orbit->polar = 15;
        }
        break;
    }

error:
    return;
}
