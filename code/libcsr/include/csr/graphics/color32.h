////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct color32
{
    union
    {
        u32 abgr;

        struct {
            u8 r, g, b, a;
        };
    };
};

////////////////////////////////////////////////////////////

// helpers
#define COLOR_RGBA32_4U(r, g, b, a) (a << 24 | b << 16 | g << 8 | r)

#define COLOR_RGBA32_4F(r, g, b, a) ( \
    (u32)(a * 255.0f) << 24 | (u32)(b * 255.0f) << 16 | (u32)(g * 255.0f) << 8 | (u32)(r * 255.0f) \
)

#define COLOR_CHAN_R(c) ((c & 0x000000ff) >> 0)
#define COLOR_CHAN_G(c) ((c & 0x0000ff00) >> 8)
#define COLOR_CHAN_B(c) ((c & 0x00ff0000) >> 16)
#define COLOR_CHAN_A(c) ((c & 0xff000000) >> 24)

// color presets
#define COLOR_BLACK     COLOR_RGBA32_4U(  0,   0,   0, 255)
#define COLOR_WHITE     COLOR_RGBA32_4U(255, 255, 255, 255)

#define COLOR_RED       COLOR_RGBA32_4U(255,   0,   0, 255)
#define COLOR_YELLOW    COLOR_RGBA32_4U(255, 255,   0, 255)
#define COLOR_BLUE      COLOR_RGBA32_4U(  0,   0, 255, 255)

#define COLOR_ORANGE    COLOR_RGBA32_4U(255, 165,   0, 255)
#define COLOR_GREEN     COLOR_RGBA32_4U(  0, 255,   0, 255)
#define COLOR_VIOLET    COLOR_RGBA32_4U(127,   0, 255, 255)

////////////////////////////////////////////////////////////

#define make_color32(...) \
    make_compound_literal(struct color32, __VA_ARGS__)

#define make_color32_3u(r, g, b)        make_color32(COLOR_RGBA32_4U(r, g, b, 255))
#define make_color32_4u(r, g, b, a)     make_color32(COLOR_RGBA32_4U(r, g, b, a))

#define make_color32_3f(r, g, b)        make_color32(COLOR_RGBA32_4F(r, g, b, 1.0))
#define make_color32_4f(r, g, b, a)     make_color32(COLOR_RGBA32_4F(r, g, b, a))

#define make_color32_from_vec4(v)       make_color32(COLOR_RGBA32_4F(v.r, v.g, v.b, v.a))

#define make_vec4_from_color32(c) make_vec4( \
    .r = COLOR_CHAN_R(c) / 255.0f, \
    .g = COLOR_CHAN_G(c) / 255.0f, \
    .b = COLOR_CHAN_B(c) / 255.0f, \
    .a = COLOR_CHAN_A(c) / 255.0f \
);

////////////////////////////////////////////////////////////

CSR_INLINE struct color32 color32_lerp(f32 value, struct color32 min, struct color32 max)
{
    return make_color32_3u(
        (u8) lerp(value, min.r, max.r),
        (u8) lerp(value, min.g, max.g),
        (u8) lerp(value, min.b, max.b)
    );
}
