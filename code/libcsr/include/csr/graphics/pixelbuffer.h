////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "color32.h"

#include <csr/core/math/rect.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct pixelbuffer
{
    u32 width;
    u32 height;

    u32 num_pixels;
    struct color32 *pixels;

    void *user_data;
};

struct pixelbuffer* pixelbuffer_create(u32 width, u32 height);
struct pixelbuffer* pixelbuffer_create_from_file(struct string filename, bool flip_y_axis);

void pixelbuffer_destroy(struct pixelbuffer* pb);
void pixelbuffer_resize(struct pixelbuffer* pb, u32 width, u32 height);

void pixelbuffer_fill(struct pixelbuffer* pb, struct color32 color);
void pixelbuffer_fill_rect(struct pixelbuffer* pb, struct rect rect, struct color32 color);

void pixelbuffer_draw_point(struct pixelbuffer* pb, u32 x, u32 y, struct color32 color);
void pixelbuffer_draw_line(struct pixelbuffer* pb, struct vec2 p1, struct vec2 p2, struct color32 color);


#define pixelbuffer_draw_point_fast(pb, x, y, color) (pb->pixels[pb->width * y + x] = color)
