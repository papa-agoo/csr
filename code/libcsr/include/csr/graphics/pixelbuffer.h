////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "color.h"

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
void pixelbuffer_destroy(struct pixelbuffer* pb);
void pixelbuffer_resize(struct pixelbuffer* pb, u32 width, u32 height);

void pixelbuffer_fill(struct pixelbuffer* pb, struct color32 color);
void pixelbuffer_set_pixel(struct pixelbuffer* pb, u32 x, u32 y, struct color32 color);
