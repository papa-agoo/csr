////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct eeq
{
    f32 a, b, c;

    bool t;
};

struct peq
{
    f32 a, b, c;
};

#define BLOCK_WIDTH 8
#define BLOCK_HEIGHT 8

struct triangle
{
    // vertices
    struct softgl_vertex *v0, *v1, *v2;

    // bounding box
    s32 xmin, xmax, xlen;
    s32 ymin, ymax, ylen;

    // edge equations
    struct eeq e0, e1, e2;

    // parameter equations
    u32 attrib_count;
    struct peq p[8 * 4]; // FIXME

    struct peq pz;
    struct peq pw;

    f32 area; // FIXME

    struct context* ctx;
};

static inline void eeq_compute(struct eeq* e, struct vec4* v0, struct vec4* v1, struct vec4* v2)
{
    f32 a = v0->y - v1->y;
    f32 b = v1->x - v0->x;
    f32 c = - (a * (v0->x + v1->x) + b * (v0->y + v1->y)) / 2.0;

    // flip coefficients for back facing triangles
    if ((a * v2->x + b * v2->y + c) < 0)
    {
        // a *= -1;
        // b *= -1;
        // c *= -1;
    }

    e->a = a;
    e->b = b;
    e->c = c;

    e->t = (a != 0) ? a > 0 : b > 0;
}

static inline bool eeq_test(struct eeq* e, f32 x, f32 y)
{
    f32 r = (e->a * x + e->b * y + e->c);

    return (r > 0 || (r == 0 && e->t));
}

static inline void peq_compute(struct peq* p, f32 area, f32 p0, f32 p1, f32 p2, struct eeq* e0, struct eeq* e1, struct eeq* e2)
{
    f32 factor = 1 / (2.0f * area);

    p->a = factor * (p0 * e0->a + p1 * e1->a + p2 * e2->a);
    p->b = factor * (p0 * e0->b + p1 * e1->b + p2 * e2->b);
    p->c = factor * (p0 * e0->c + p1 * e1->c + p2 * e2->c);
}

static inline f32 peq_value(struct peq* p, f32 x, f32 y)
{
    return p->a * x + p->b * y + p->c;
}

static inline void triangle_setup(struct triangle* t, struct softgl_vertex* a, struct softgl_vertex* b, struct softgl_vertex* c, bool extend_aabb)
{
    t->v0 = a; t->v1 = b; t->v2 = c;

    t->attrib_count = a->attribs_out.count;

    ////////////////////////////////////////

    // compute bounding box
    t->xmin = min(min(a->position.x, b->position.x), c->position.x);
    t->xmax = max(max(a->position.x, b->position.x), c->position.x);

    t->ymin = min(min(a->position.y, b->position.y), c->position.y);
    t->ymax = max(max(a->position.y, b->position.y), c->position.y);

    t->xlen = t->xmax - t->xmin;
    t->ylen = t->ymax - t->ymin;

    // extend bounding box to multiple of block size
    if (extend_aabb)
    {
        t->xmax = t->xmin + ((t->xlen + 7) & (-BLOCK_WIDTH));
        t->ymax = t->ymin + ((t->ylen + 7) & (-BLOCK_HEIGHT));
    }

    // clip to viewport (FIXME: scissor box?)
    struct softgl_viewport* vp = &softgl_state_ptr()->viewport;

    t->xmin = max(vp->x, t->xmin);
    t->xmax = min(vp->width - 1, t->xmax);

    t->ymin = max(vp->y, t->ymin);
    t->ymax = min(vp->height - 1, t->ymax);

    ////////////////////////////////////////

    // compute edge equations
    eeq_compute(&t->e0, &a->position, &b->position, &c->position);
    eeq_compute(&t->e1, &b->position, &c->position, &a->position);
    eeq_compute(&t->e2, &c->position, &a->position, &b->position);

    // compute triangle area
    t->area = 0.5 * (t->e0.c + t->e1.c + t->e2.c);

    ////////////////////////////////////////

    // compute paramenter equations
    for (u32 i = 0; i < t->attrib_count; i++)
    {
        struct vec4 *a0 = &a->attribs_out.slots[i];
        struct vec4 *a1 = &b->attribs_out.slots[i];
        struct vec4 *a2 = &c->attribs_out.slots[i];

        struct peq* p = t->p + (i * 4);

        peq_compute(p++, t->area, a0->x, a1->x, a2->x, &t->e1, &t->e2, &t->e0);
        peq_compute(p++, t->area, a0->y, a1->y, a2->y, &t->e1, &t->e2, &t->e0);
        peq_compute(p++, t->area, a0->z, a1->z, a2->z, &t->e1, &t->e2, &t->e0);
        peq_compute(p++, t->area, a0->w, a1->w, a2->w, &t->e1, &t->e2, &t->e0);
    }

    peq_compute(&t->pz, t->area, a->position.z, b->position.z, c->position.z, &t->e1, &t->e2, &t->e0);
    peq_compute(&t->pw, t->area, a->position.w, b->position.w, c->position.w, &t->e1, &t->e2, &t->e0);
}

static inline void fragment_setup(struct softgl_fragment* f, struct triangle* t, u32 x, u32 y)
{
    f->frag_coords.x = x;
    f->frag_coords.y = y;
    f->frag_coords.z = peq_value(&t->pz, x, y);
    f->frag_coords.w = peq_value(&t->pw, x, y);

    for (u32 i = 0; i < t->attrib_count; i++)
    {
        f32* data = f->attribs_in.slots[i].data;

        struct peq* p = t->p + (i * 4);

        data[0] = peq_value(p++, x, y);
        data[1] = peq_value(p++, x, y);
        data[2] = peq_value(p++, x, y);
        data[3] = peq_value(p++, x, y);
    }
}

////////////////////////////////////////////////////////////////////////////////

static inline bool point_inside_triangle(struct triangle* t, f32 x, f32 y)
{
    return eeq_test(&t->e0, x, y) && eeq_test(&t->e1, x, y) && eeq_test(&t->e2, x, y);
}

static inline u32 block_inside_triangle(struct triangle* t, f32 px, f32 py)
{
    u32 count = 0;

    // bottom left
    if (point_inside_triangle(t, px, py)) count++;

    // top left
    if (point_inside_triangle(t, px, py + BLOCK_HEIGHT)) count++;

    // top right
    if (point_inside_triangle(t, px + BLOCK_WIDTH, py + BLOCK_HEIGHT)) count++;

    // bottom right
    if (point_inside_triangle(t, px + BLOCK_WIDTH, py)) count++;

    return count;
}

inline bool point_inside_block(struct triangle* t, f32 x, f32 y)
{
    return false; // FIXME
}
