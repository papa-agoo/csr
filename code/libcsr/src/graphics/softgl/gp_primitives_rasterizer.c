////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gp.h"

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

void gp_process_fragment(struct softgl_fragment* fragment)
{
    struct pixelbuffer *pb = softgl_state_ptr()->pb;

    ////////////////////////////////////////

    u32 p_y = fragment->frag_coords.y;
    u32 p_x = fragment->frag_coords.x;

    // early fragment discard
    if (p_x < 0 || p_x >= pb->width || p_y < 0 || p_y >= pb->height) return;

    // early depth test
    struct softgl_depth_state *ds = &softgl_pso_ptr()->depth_stencil_state.depth;

    if (ds->enable_test)
    {
        // u32 frag_idx = (pb->width * p_y) + p_x;

        // f32* z_new = &fragment->frag_coords.z;
        // f32* z_old = pb->user_data + frag_idx;

        // if (*z_new < *z_old) return;

        // *z_old = *z_new;
    }

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

    gp_process_fragment(&fragment);
}

void gp_rasterize_line(struct softgl_vertex* a, struct softgl_vertex* b)
{
    s32 x1 = a->position.x;
    s32 y1 = a->position.y;

    s32 x2 = b->position.x;
    s32 y2 = b->position.y;

    ////////////////////////////////////////

    // deltas for x and y
    s32 delta_x = abs(x2 - x1);
    s32 delta_y = -abs(y2 - y1);

    // steps for x and y
    s32 step_x = (x1 < x2) ? 1 : -1;
    s32 step_y = (y1 < y2) ? 1 : -1;

    // step error
    s32 step_error = delta_x + delta_y;

    ////////////////////////////////////////

    // FIXME bresenham for now, will do edge equations next (no interpolation of depth or vertex attributes)
    struct softgl_fragment fragment = {0};
    fragment.frag_coords = a->position;
    fragment.attribs_in = a->attribs_out;

    while (1)
    {
        fragment.frag_coords.x = x1;
        fragment.frag_coords.y = y1;

        gp_process_fragment(&fragment);

        if (x1 == x2 && y1 == y2) break;

        s32 error = step_error << 1;

        // x axis
        if (error > delta_y)
        {
            step_error += delta_y;
            x1 += step_x;
        }

        // y axis
        if (error < delta_x)
        {
            step_error += delta_x;
            y1 += step_y;
        }
    }
}
