////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gp.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  FRAGMENT PIPELINE FLOW
// 
// - gp_process_primitives
// 
//      - process_points
//         << LOOP >>
//              - clip_point
//              - project_to_screen
//              - rasterize_point
//                  - process_fragment
// 
//      - process_lines
//          << LOOP >>
//              - clip_line
//              - project_to_screen
//              - rasterize_line
//                  - setup_line
//                  - rasterize_line_impl
//                      <<LOOP>>
//                          - interpolate data
//                          - process_fragment
// 
//      - process_triangles
//          << LOOP >>
//              - clip_triangle
//              - (handle new vertices produced by clipping process)
//              - project_to_screen
//              - rasterize_triangle
//                  - setup_triangle
//                  - rasterize_triangle_impl
//                      <<LOOP>>
//                          - interpolate data
//                          - process_fragment
// 
////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  FUNCTIONS
// 
// (F) setup_line()
//      - calc aabb
//      - ...
//
// (F) setup_triangle()
//      - calc aabb
//      - clip aabb to some rect (viewport, ie.)
//      - calc edge functions (interpolation coefficients)
// 
// (F) project_to_screen()
//      - perspective divide
//      - viewport transform
// 
// (F) process_fragment()
//      - early depth test
//      - exec fragment shader
//      - merge_output()
// 
// (F) merge_output()
//      - scissor test ?
//      - stencil test ?
//      - depth test ?
//      - blend colors ?
//      - set pixel
// 
////////////////////////////////////////////////////////////////////////////////////////////////////

static void _project_to_screen(struct softgl_vertex* v)
{
    // position in clip space
    struct vec4* p = &v->position;

    // perspective divide (clip -> ndc)
    p->x /= p->w;
    p->y /= p->w;
    p->z /= p->w;
    p->w = 1.0f / p->w;

    ////////////////////////////////////////

    struct softgl_viewport* vp = &softgl_state_ptr()->viewport;

    // remap ndc [-1, 1] to viewport [0, w - 1], [0, h - 1]
    p->x = (p->x + 1.0f) * ((vp->width - 1.0f) * 0.5f) + vp->x;
    p->y = (p->y + 1.0f) * ((vp->height - 1.0f) * 0.5f) + vp->y;

    // remap z from [-1, 1] to [0, 1]
    p->z = (p->z + 1.0f) * 0.5f;
}

static void _process_points()
{
    struct primitive_cache* pc = &softgl_gp_ptr()->pc;

    for (u32 i = 0; i < pc->vertex_count; i++)
    {
        struct softgl_vertex* a = pc->vertices[i];

        if (gp_clip_point(a) == 0) continue;

        _project_to_screen(a);

        gp_rasterize_point(a);
    }
}

static void _process_lines()
{
    struct primitive_cache* pc = &softgl_gp_ptr()->pc;

    for (u32 i = 0; i < pc->vertex_count - 1; i += 2)
    {
        struct softgl_vertex* a = pc->vertices[i];
        struct softgl_vertex* b = pc->vertices[i+1];

        // FIXME < 2?
        if (gp_clip_line(a, b) < 2) continue;

        _project_to_screen(a);
        _project_to_screen(b);

        gp_rasterize_line(a, b);
    }
}

static void _process_triangles()
{
    struct primitive_cache* pc = &softgl_gp_ptr()->pc;

    for (u32 i = 0; i < pc->vertex_count - 2; i += 3)
    {
        struct softgl_vertex* a = pc->vertices[i];
        struct softgl_vertex* b = pc->vertices[i+1];
        struct softgl_vertex* c = pc->vertices[i+2];

        // FIXME < 3?
        if (gp_clip_triangle(a, b, c) < 3) continue;

        // FIXME handle num_triangles > 1

        _project_to_screen(a);
        _project_to_screen(b);
        _project_to_screen(c);

        gp_rasterize_triangle(a, b, c);
    }
}

void gp_process_primitives()
{
    switch (softgl_pso_ptr()->topology)
    {
        // points
        case SOFTGL_TOPOLOGY_POINT_LIST:
            _process_points();
        break;

        // lines
        case SOFTGL_TOPOLOGY_LINE_LIST:
        case SOFTGL_TOPOLOGY_LINE_STRIP:
        case SOFTGL_TOPOLOGY_LINE_LOOP:
            _process_lines();
        break;

        // triangles
        case SOFTGL_TOPOLOGY_TRIANGLE_LIST:
        case SOFTGL_TOPOLOGY_TRIANGLE_STRIP:
        case SOFTGL_TOPOLOGY_TRIANGLE_FAN:
            _process_triangles();
        break;
    }
}
