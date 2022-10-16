////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// signed ints
typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

// unsigned ints
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

// real numbers
typedef float       f32;
typedef double      f64;

// compound type helpers
#define  make_s8(v) make_compound_literal( s8, v)
#define make_s16(v) make_compound_literal(s16, v)
#define make_s32(v) make_compound_literal(s32, v)
#define make_s64(v) make_compound_literal(s64, v)

#define  make_u8(v) make_compound_literal( u8, v)
#define make_u16(v) make_compound_literal(u16, v)
#define make_u32(v) make_compound_literal(u32, v)
#define make_u64(v) make_compound_literal(u64, v)

#define make_f32(v) make_compound_literal(f32, v)
#define make_f64(v) make_compound_literal(f64, v)
