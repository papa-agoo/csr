////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct vec3
{
    union
    {
        f32 data[3];

        struct {
            f32 x, y, z;
        };

        struct {
            f32 r, g, b;
        };
    };
};

#define make_vec3(...) \
    make_compound_literal(struct vec3, __VA_ARGS__)

#define make_vec3_2_1(_v2, z)   make_vec3((_v2).x, (_v2).y, z)

#define make_vec3_zero()        make_vec3(0, 0, 0)
#define make_vec3_one()         make_vec3(1, 1, 1)

#define make_vec3_x_axis()      make_vec3(1, 0, 0)
#define make_vec3_y_axis()      make_vec3(0, 1, 0)
#define make_vec3_z_axis()      make_vec3(0, 0, 1)

#define make_vec3_up()          make_vec3_y_axis()
#define make_vec3_right()       make_vec3_x_axis()
#define make_vec3_forward()     make_vec3(0, 0, -1)

////////////////////////////////////////////////////////////////////////////////

CSR_INLINE struct vec3 vec3_add(struct vec3 a, struct vec3 b)
{
    return make_vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

CSR_INLINE struct vec3 vec3_sub(struct vec3 a, struct vec3 b)
{
    return make_vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

CSR_INLINE f32 vec3_length(struct vec3 a)
{
    // FIXME fast sqrt
    return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

CSR_INLINE f32 vec3_distance(struct vec3 a, struct vec3 b)
{
    return vec3_length( vec3_sub(a, b) );
}

CSR_INLINE struct vec3 vec3_scale(struct vec3 a, f32 factor)
{
    return make_vec3(
        a.x * factor,
        a.y * factor,
        a.z * factor
    );
}

CSR_INLINE struct vec3 vec3_normalize(struct vec3 a)
{
    f32 length = vec3_length(a);

    if (length == 0)
        return a;

    return vec3_scale(a, 1.0f / length);
}

CSR_INLINE f32 vec3_dot(struct vec3 a, struct vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

CSR_INLINE struct vec3 vec3_cross(struct vec3 a, struct vec3 b)
{
    return make_vec3(
        .x = (a.y * b.z) - (a.z * b.y),
        .y = (a.z * b.x) - (a.x * b.z),
        .z = (a.x * b.y) - (a.y * b.x)
    );
}

CSR_INLINE struct vec3 vec3_reflect(struct vec3 i, struct vec3 n)
{
    // I - 2.0 * dot(N, I) * N
    f32 dot = vec3_dot(n, i);

    return make_vec3(
        .x = i.x - 2.0f * dot * n.x,
        .y = i.y - 2.0f * dot * n.y,
        .z = i.z - 2.0f * dot * n.z
    );
}

CSR_INLINE struct vec3 vec3_lerp(struct vec3 a, struct vec3 b, f32 value)
{
    return make_vec3(
        .x = lerp(value, a.x, b.x),
        .y = lerp(value, a.y, b.y),
        .z = lerp(value, a.z, b.z)
    );
}

// FIXME vec3_slerp()

CSR_INLINE struct vec3 vec3_clamp(struct vec3 a, f32 min, f32 max)
{
    return make_vec3(
        .x = clamp(a.x, min, max),
        .y = clamp(a.y, min, max),
        .z = clamp(a.z, min, max)
    );
}

CSR_INLINE bool vec3_equal(struct vec3 a, struct vec3 b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

CSR_INLINE f32 vec3_angle(struct vec3 a, struct vec3 b)
{
    a = vec3_normalize(a);
    b = vec3_normalize(b);

    return RAD2DEG(acosf(vec3_dot(a, b)));
}

CSR_INLINE struct vec3 vec3_direction(struct vec3 a, struct vec3 b)
{
    return vec3_normalize(vec3_sub(b, a));
}
