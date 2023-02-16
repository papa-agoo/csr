////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/pixelbuffer.h>

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

    // release user data (ie. depth_stencil)
    if (pb->user_data) {
        free(pb->user_data);
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

void pixelbuffer_set_pixel(struct pixelbuffer* pb, u32 x, u32 y, struct color32 color)
{
    csr_assert(pb != NULL);

    csr_assert(x >= 0 && x < pb->width);
    csr_assert(y >= 0 && y < pb->height);

    u32 offset = (pb->width * y) + x;

    pb->pixels[offset] = color;

error:
    return;
}
