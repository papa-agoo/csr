////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/math/utils.h>
#include <csr/graphics/pixelbuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define PIXEL_OFFSET_IDX(pb, x, y) ((pb->width * y) + x)
#define PIXEL_OFFSET_PTR(pb, x, y) (pb->pixels + PIXEL_OFFSET_IDX(pb, x, y))

CSR_INLINE void _rasterize_point(struct pixelbuffer *pb, u32 x, u32 y, struct color32 color)
{
    *PIXEL_OFFSET_PTR(pb, x, y) = color;
}

CSR_INLINE void _rasterize_line(struct pixelbuffer* pb, struct vec2 p1, struct vec2 p2, struct color32 color)
{
    // https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
    u32 x_min = min(p1.x, p2.x);
    u32 x_max = max(p1.x, p2.x);

    u32 y_min = min(p1.y, p2.y);
    u32 y_max = max(p1.y, p2.y);

    ////////////////////////////////////////

    f32 dx = p2.x - p1.x;
    f32 dy = p2.y - p1.y;

    f32 m = dy / dx;

    f32 delta = fabsf(m);
    f32 delta_1od = fabsf(1 / delta);

    f32 threshold = 0.5;
    f32 offset = 0.0;

    ////////////////////////////////////////

    if (m >= -1 && m <= 1)
    {
        u32 y = (m > 0) ? y_min : y_max;
        u32 y_step = (m > 0) ? 1 : -1;

        for (u32 x = x_min; x <= x_max; x++)
        {
            if (offset > threshold)
            {
                y += y_step;
                threshold += 1;
            }

            offset += delta;

            _rasterize_point(pb, x, y, color);
        }
    }
    else
    {
        delta = delta_1od;

        u32 x = (m > 1) ? x_min : x_max;
        u32 x_step = (m > 1) ? 1 : -1;

        for (u32 y = y_min; y <= y_max; y++)
        {
            if (offset >= threshold)
            {
                x += x_step;
                threshold += 1;
            }

            offset += delta;

            _rasterize_point(pb, x, y, color);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct pixelbuffer* pixelbuffer_create(u32 width, u32 height)
{
    check_expr(width > 0);
    check_expr(height > 0);

    struct pixelbuffer* pb = calloc(1, sizeof(struct pixelbuffer));
    check_mem(pb);

    pixelbuffer_resize(pb, width, height);

    return pb;

error:
    return NULL;
}

void pixelbuffer_destroy(struct pixelbuffer *pb)
{
    check_ptr(pb);

    // release color buffer
    if (pb->pixels) {
        free(pb->pixels);
    }

    if (pb->user_data) {
        clog_warn("pixelbuffer: user_data still allocated!");
    }

error:
    free(pb);

    return;
}

void pixelbuffer_resize(struct pixelbuffer* pb, u32 width, u32 height)
{
    check_ptr(pb);

    check_expr(width > 0);
    check_expr(height > 0);

    pb->width = width;
    pb->height = height;

    pb->num_pixels = width * height;

    pb->pixels = realloc(pb->pixels, pb->num_pixels * sizeof(u32));
    check_mem(pb->pixels);

error:
    return;
}

void pixelbuffer_fill(struct pixelbuffer* pb, struct color32 color)
{
    check_ptr(pb);

    check_expr(pb->num_pixels > 0);

    wmemset((wchar_t*)pb->pixels, color.abgr, pb->num_pixels);

error:
    return;
}

void pixelbuffer_fill_rect(struct pixelbuffer* pb, struct rect rect, struct color32 color)
{
    check_ptr(pb);

    check_expr(pb->num_pixels > 0);

    // FIXME clip rect

    for (u32 y = rect.y; y < rect.y + rect.h; y++) {
        wmemset((wchar_t*)PIXEL_OFFSET_PTR(pb, rect.x, y), color.abgr, rect.w);
    }

error:
    return;
}

void pixelbuffer_draw_point(struct pixelbuffer* pb, u32 x, u32 y, struct color32 color)
{
    check_ptr(pb);

    // clip point
    if (x < 0 || x >= pb->width || y < 0 || y >= pb->height) return;

    _rasterize_point(pb, x, y, color);

error:
    return;
}

void pixelbuffer_draw_line(struct pixelbuffer* pb, struct vec2 p1, struct vec2 p2, struct color32 color)
{
    check_ptr(pb);

    // FIXME clip line

    _rasterize_line(pb, p1, p2, color);

error:
    return;
}
