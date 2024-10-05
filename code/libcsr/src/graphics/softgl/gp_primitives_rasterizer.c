////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gp.h"
#include "triangle.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _merge_output(struct softgl_fragment *fragment)
{
    struct pixelbuffer *pb = softgl_state_ptr()->pb;

    struct vec4 color = fragment->color;

    // ... scissor, stencil, depth, blend ...

    struct color32 final_color = make_color32(COLOR_RGBA32_4F(color.r, color.g, color.b, color.a));

    // write fragment color to the pixel buffer :)
    u32 p_x = fragment->frag_coords.x;
    u32 p_y = fragment->frag_coords.y;

    pixelbuffer_draw_point_fast(pb, p_x, p_y, final_color);
}

static void _process_fragment(struct softgl_fragment* fragment)
{
    struct pixelbuffer *pb = softgl_state_ptr()->pb;

    // integer coords needed for memory offset calc.
    s32 p_y = fragment->frag_coords.y;
    s32 p_x = fragment->frag_coords.x;

    u32 offset = (pb->width * p_y) + p_x;

    ////////////////////////////////////////

    // early depth test
    // struct softgl_depth_state *ds = &softgl_pso_ptr()->depth_stencil_state.depth;

    // if (ds->enable_test)
    // {
    //     f32 *depth_stencil_buffer = pb->user_data;

    //     f32* z_new = &fragment->frag_coords.z;
    //     f32* z_old = depth_stencil_buffer + offset;

    //     if (*z_new > *z_old) return;

    //     *z_old = *z_new;
    // }

    ////////////////////////////////////////

    // execute fragment shader
    softgl_pso_ptr()->shader->fragment_shader_ptr(fragment);

    // merge fragment
    _merge_output(fragment);
}

void gp_rasterize_point(struct softgl_vertex* a)
{
    struct softgl_fragment fragment = {0};
    fragment.frag_coords = a->position;
    fragment.attribs_in = a->attribs_out;
    fragment.front_facing = true;

    _process_fragment(&fragment);
}

void gp_rasterize_line(struct softgl_vertex* a, struct softgl_vertex* b)
{
    u32 x1 = a->position.x;
    u32 y1 = a->position.y;

    u32 x2 = b->position.x;
    u32 y2 = b->position.y;

    ////////////////////////////////////////

    // deltas for x and y
    s32 dx = abs(x2 - x1);
    s32 dy = -abs(y2 - y1);

    // steps for x and y
    s32 sx = (x1 < x2) ? 1 : -1;
    s32 sy = (y1 < y2) ? 1 : -1;

    // step error
    s32 se = dx + dy;

    ////////////////////////////////////////

    // FIXME bresenham for now, will do edge equations next
    struct softgl_fragment fragment = {0};
    fragment.attribs_in = b->attribs_out;

    while (1)
    {
        fragment.frag_coords.x = x1;
        fragment.frag_coords.y = y1;

        _process_fragment(&fragment);

        if (x1 == x2 && y1 == y2) break;

        s32 error = se << 1;

        // x axis
        if (error > dy)
        {
            se += dy;
            x1 += sx;

            // fragment_step_x();
        }

        // y axis
        if (error < dx)
        {
            se += dx;
            y1 += sy;

            // fragment_step_y();
        }
    }
}

void gp_rasterize_triangle(struct softgl_vertex* a, struct softgl_vertex* b, struct softgl_vertex* c)
{
    struct triangle triangle = {0};
    triangle_setup(&triangle, a, b, c, false);

    if (triangle.area <= 0) return; // FIXME

    ////////////////////////////////////////

    struct softgl_fragment fragment = {0};
    fragment.front_facing = triangle.area > 0;
    fragment.attribs_in.count = triangle.attrib_count;

    f32 px = 0;
    f32 py = 0;

    for (s32 y = triangle.ymin; y < triangle.ymax; y++)
    {
        py = y + 1;

        for (s32 x = triangle.xmin; x < triangle.xmax; x++)
        {
            px = x + 1;

            if (point_inside_triangle(&triangle, px, py))
            {
                fragment_setup(&fragment, &triangle, px, py);

                _process_fragment(&fragment);
            }
        }
    }
}

// void gp_rasterize_line(struct softgl_vertex* a, struct softgl_vertex* b)
// {
//     gp_rasterize_point(a);
//     gp_rasterize_point(b);

//     // ...
// }

// void gp_rasterize_triangle(struct softgl_vertex* a, struct softgl_vertex* b, struct softgl_vertex* c)
// {
//     gp_rasterize_point(a);
//     gp_rasterize_point(b);
//     gp_rasterize_point(c);

//     // ...
// }
