////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct vec4
{
    union
    {
        f32 data[4];

        struct {
            f32 x, y, z, w;
        };

        struct {
            f32 r, g, b, a;
        };

        struct {
            f32 u, v, s, t;
        };
    };
};

#define make_vec4(...) \
    make_compound_literal(struct vec4, __VA_ARGS__)

#define make_vec4_2_2(_v2, z, w)    make_vec4((_v2).x, (_v2).y, z, w)
#define make_vec4_3_1(_v3, w)       make_vec4((_v3).x, (_v3).y, (_v3).z, w)

////////////////////////////////////////////////////////////////////////////////

CSR_INLINE struct vec4 vec4_add(struct vec4 a, struct vec4 b)
{
    return make_vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

CSR_INLINE struct vec4 vec4_sub(struct vec4 a, struct vec4 b)
{
    return make_vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

CSR_INLINE f32 vec4_length(struct vec4 a)
{
    // FIXME fast sqrt
    return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z) + (a.w * a.w));
}

CSR_INLINE f32 vec4_distance(struct vec4 a, struct vec4 b)
{
    return vec4_length( vec4_sub(a, b) );
}

CSR_INLINE struct vec4 vec4_scale(struct vec4 a, f32 factor)
{
    return make_vec4(
        a.x * factor,
        a.y * factor,
        a.z * factor,
        a.w * factor
    );
}

CSR_INLINE struct vec4 vec4_normalize(struct vec4 a)
{
    f32 length = vec4_length(a);

    if (length == 0)
        return a;

    return vec4_scale(a, 1.0f / length);
}

CSR_INLINE struct vec4 vec4_lerp(struct vec4 a, struct vec4 b, f32 value)
{
    return make_vec4(
        .x = lerp(value, a.x, b.x),
        .y = lerp(value, a.y, b.y),
        .z = lerp(value, a.z, b.z),
        .w = lerp(value, a.w, b.w)
    );
}

CSR_INLINE struct vec4 vec4_clamp(struct vec4 a, f32 min, f32 max)
{
    return make_vec4(
        .x = clamp(a.x, min, max),
        .y = clamp(a.y, min, max),
        .z = clamp(a.z, min, max),
        .w = clamp(a.w, min, max)
    );
}

CSR_INLINE bool vec4_equal(struct vec4 a, struct vec4 b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}
