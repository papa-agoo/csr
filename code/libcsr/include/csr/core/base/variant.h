////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"

#include <csr/core/math/vec2.h>
#include <csr/core/math/vec3.h>
#include <csr/core/math/vec4.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum variant_type
{
    VARIANT_TYPE_NULL,

    VARIANT_TYPE_BYTE,

    VARIANT_TYPE_S8,
    VARIANT_TYPE_S16,
    VARIANT_TYPE_S32,
    VARIANT_TYPE_S64,

    VARIANT_TYPE_F32,
    VARIANT_TYPE_F64,

    VARIANT_TYPE_VEC2,
    VARIANT_TYPE_VEC3,
    VARIANT_TYPE_VEC4,

    VARIANT_TYPE_BOOL,

    VARIANT_TYPE_STR,
    VARIANT_TYPE_PTR,
};

struct variant
{
    enum variant_type type;

    union {
        u8 as_byte;

        s8 as_s8;
        s16 as_s16;
        s32 as_s32;
        s64 as_s64;

        f32 as_f32;
        f64 as_f64;

        struct vec2 as_vec2;
        struct vec3 as_vec3;
        struct vec4 as_vec4;

        bool as_bool;

        const char* as_str;
        void* as_ptr;
    };
};

////////////////////////////////////////////////////////////////////////////////

#define make_variant(...) \
    make_compound_literal(struct variant, __VA_ARGS__)

#define make_variant_byte(v)            make_variant(VARIANT_TYPE_BYTE, .as_u8 = v)

#define make_variant_s8(v)              make_variant(VARIANT_TYPE_S8, .as_s8 = v)
#define make_variant_s16(v)             make_variant(VARIANT_TYPE_S16, .as_s16 = v)
#define make_variant_s32(v)             make_variant(VARIANT_TYPE_S32, .as_s32 = v)
#define make_variant_s64(v)             make_variant(VARIANT_TYPE_S64, .as_s64 = v)

#define make_variant_f32(v)             make_variant(VARIANT_TYPE_F32, .as_f32 = v)
#define make_variant_f64(v)             make_variant(VARIANT_TYPE_F64, .as_f64 = v)

#define make_variant_vec2(x, y)         make_variant(VARIANT_TYPE_VEC2, .as_vec2 = make_vec2(x, y))
#define make_variant_vec3(x, y, z)      make_variant(VARIANT_TYPE_VEC3, .as_vec3 = make_vec3(x, y, z))
#define make_variant_vec4(x, y, z, w)   make_variant(VARIANT_TYPE_VEC4, .as_vec4 = make_vec4(x, y, z, w))

#define make_variant_bool(v)            make_variant(VARIANT_TYPE_BOOL, .as_bool = v)

#define make_variant_str(v)             make_variant(VARIANT_TYPE_STR, .as_str = strdup(v))
#define make_variant_ptr(v)             make_variant(VARIANT_TYPE_PTR, .as_ptr = (void*) v)
