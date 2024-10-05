////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gp.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

u32 gp_clip_point(struct softgl_vertex* a)
{
    struct vec4* pa = &a->position;

    // if (pa->x < -pa->w || pa->x > pa->w) return 0;
    // if (pa->y < -pa->w || pa->y > pa->w) return 0;
    if (pa->z < -pa->w || pa->z > pa->w) return 0;

    return 1;
}

u32 gp_clip_line(struct softgl_vertex* a, struct softgl_vertex* b)
{
    // FIXME
    return gp_clip_point(a) + gp_clip_point(b);
}

u32 gp_clip_triangle(struct softgl_vertex* a, struct softgl_vertex* b, struct softgl_vertex* c)
{
    // FIXME
    return gp_clip_point(a) + gp_clip_point(b) + gp_clip_point(c);
}
