////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vec3.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct aabb
{
    struct vec3 min;
    struct vec3 max;
};

#define make_aabb(...) \
    make_compound_literal(struct aabb, __VA_ARGS__)

#define make_aabb_zero() \
    make_aabb(make_vec3_zero(), make_vec3_zero());

#define make_aabb_unit_cube() \
    make_aabb(make_vec3(-1, -1, -1), make_vec3_one());

////////////////////////////////////////////////////////////////////////////////

CSR_INLINE bool aabb_is_valid(const struct aabb *aabb)
{
    check_ptr(aabb);

    clog_warn("aabb_is_valid() not impl. yet");

error:
    return false;
}

CSR_INLINE struct vec3 aabb_get_center(const struct aabb *aabb)
{
    check_ptr(aabb);

    struct vec3 min_to_max = vec3_sub(aabb->max, aabb->min);

    return vec3_add(aabb->min, vec3_scale(min_to_max, 0.5));

error:
    return make_vec3_zero();
}

CSR_INLINE struct vec3 aabb_get_bottom(const struct aabb *aabb)
{
    check_ptr(aabb);

    struct vec3 center = aabb_get_center(aabb);
    center.y = aabb->min.y;

    return center;

error:
    return make_vec3_zero();
}

CSR_INLINE struct vec3 aabb_scale_to_box(const struct aabb *aabb, struct vec3 box_dst)
{
    check_ptr(aabb);

    struct vec3 box_src = vec3_sub(aabb->max, aabb->min);

    f32 box_src_len = max(box_src.x, box_src.y);
    box_src_len = max(box_src_len, box_src.z);

    f32 box_dst_len = max(box_dst.x, box_dst.y);
    box_dst_len = max(box_dst_len, box_dst.z);

    f32 scale_factor = fabsf(box_dst_len / box_src_len);

    return make_vec3(scale_factor, scale_factor, scale_factor);

error:
    return make_vec3_one();
}

CSR_INLINE struct vec3 aabb_box_from_aabb(const struct aabb *aabb)
{
    check_ptr(aabb);

    struct vec3 box = vec3_sub(aabb->max, aabb->min);

    return make_vec3(fabsf(box.x), fabsf(box.y), fabsf(box.z));

error:
    return make_vec3_one();
}

CSR_INLINE struct vec3 aabb_center_to_position(const struct aabb *aabb, struct vec3 position)
{
    check_ptr(aabb);

    return vec3_scale(aabb_get_center(aabb), -1);

error:
    return make_vec3_zero();
}

CSR_INLINE struct vec3 aabb_bottom_to_position(const struct aabb *aabb, struct vec3 position)
{
    check_ptr(aabb);

    return vec3_scale(aabb_get_bottom(aabb), -1);

error:
    return make_vec3_zero();
}
