////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct vec2
{
    union
    {
        f32 data[2];

        struct {
            f32 x, y;
        };

        struct {
            f32 w, h;
        };

        struct {
            f32 u, v;
        };

        struct {
            f32 min, max;
        };
    };
};

#define make_vec2(...) \
    make_compound_literal(struct vec2, __VA_ARGS__)

#define make_vec2_zero()        make_vec2(0, 0)
#define make_vec2_one()         make_vec2(1, 1)

#define make_vec2_x_axis()      make_vec2(1, 0)
#define make_vec2_y_axis()      make_vec2(0, 1)

////////////////////////////////////////////////////////////////////////////////

CSR_INLINE struct vec2 vec2_add(struct vec2 a, struct vec2 b)
{
    return make_vec2(a.x + b.x, a.y + b.y);
}

CSR_INLINE struct vec2 vec2_sub(struct vec2 a, struct vec2 b)
{
    return make_vec2(a.x - b.x, a.y - b.y);
}

CSR_INLINE f32 vec2_length(struct vec2 a)
{
    return sqrtf((a.x * a.x) + (a.y * a.y));
}

CSR_INLINE f32 vec2_distance(struct vec2 a, struct vec2 b)
{
    return vec2_length( vec2_sub(a, b) );
}

CSR_INLINE struct vec2 vec2_scale(struct vec2 a, f32 factor)
{
    return make_vec2(a.x * factor, a.y * factor);
}

CSR_INLINE struct vec2 vec2_normalize(struct vec2 a)
{
    f32 length = vec2_length(a);

    if (length == 0)
        return a;

    return vec2_scale(a, 1.0f / length);
}

CSR_INLINE f32 vec2_dot(struct vec2 a, struct vec2 b)
{
    return (a.x * b.x) + (a.y * b.y);
}

CSR_INLINE struct vec2 vec2_reflect(struct vec2 i, struct vec2 n)
{
    // I - 2.0 * dot(N, I) * N
    f32 dot = vec2_dot(n, i);

    return make_vec2(
        .x = i.x - 2.0f * dot * n.x,
        .y = i.y - 2.0f * dot * n.y
    );
}

CSR_INLINE struct vec2 vec2_lerp(struct vec2 a, struct vec2 b, f32 value)
{
    return make_vec2(
        .x = lerp(value, a.x, b.x),
        .y = lerp(value, a.y, b.y)
    );
}

CSR_INLINE struct vec2 vec2_clamp(struct vec2 a, f32 min, f32 max)
{
    return make_vec2(
        .x = clamp(a.x, min, max),
        .y = clamp(a.y, min, max)
    );
}

CSR_INLINE bool vec2_equal(struct vec2 a, struct vec2 b)
{
    return a.x == b.x && a.y == b.y;
}
