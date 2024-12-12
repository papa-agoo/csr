////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rsx_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
// debug primitives
////////////////////////////////////////////////////////////////////////////////////////////////////
void rsx_debug_add_point(struct vec3 p, struct vec3 color, f32 size, f32 lifetime, bool depth)
{
    struct rsx_pass_debug_primitives *pass_data = rsx_pass_data_debug_primitives_ptr();

    // struct vertex_1p1c v = {.position = p, .color = color};

    // u32 size_idx = clamp(size, 1, PRIMITIVE_SIZE_MAX) - 1;

    // struct mesh_buffer *buffer = (depth) ? &rsx->debug_draw.points[size_idx] : &rsx->debug_draw.points_no_depth[size_idx];

    // vector_push_back(buffer->vertices.cpu, v);

error:
    return;
}

void rsx_debug_add_line(struct vec3 a, struct vec3 b, struct vec3 color, f32 width, f32 lifetime, bool depth)
{
    struct rsx_pass_debug_primitives *pass_data = rsx_pass_data_debug_primitives_ptr();

    // struct vertex_1p1c va = {.position = a, .color = color};
    // struct vertex_1p1c vb = {.position = b, .color = color};

    // u32 size_idx = clamp(width, 1, PRIMITIVE_SIZE_MAX) - 1;

    // struct mesh_buffer *buffer = (depth) ? &rsx->debug_draw.lines[size_idx] : &rsx->debug_draw.lines_no_depth[size_idx];

    // vector_push_back(buffer->vertices.cpu, va);
    // vector_push_back(buffer->vertices.cpu, vb);

error:
    return;
}

void rsx_debug_add_axes(struct mat44 transform, bool depth)
{
    struct rsx *rsx = rsx_ptr();

    ////////////////////////////////////////

    struct vec3 origin  = mat44_mult_vec3(transform, make_vec3_zero());
    struct vec3 basis_x = mat44_mult_vec3(transform, make_vec3_x_axis());
    struct vec3 basis_y = mat44_mult_vec3(transform, make_vec3_y_axis());
    struct vec3 basis_z = mat44_mult_vec3(transform, make_vec3_z_axis());

    ////////////////////////////////////////

    f32 width = 2.0f;
    f32 lifetime = 0.0f;

    const struct rsx_conf *conf = rsx_get_conf();

    rsx_debug_add_line(origin, basis_x, conf->color.axis_x, width, lifetime, depth);
    rsx_debug_add_line(origin, basis_y, conf->color.axis_y, width, lifetime, depth);
    rsx_debug_add_line(origin, basis_z, conf->color.axis_z, width, lifetime, depth);

error:
    return;
}

void rsx_debug_add_aabb(struct mat44 transform, struct aabb aabb, bool depth)
{
    struct rsx *rsx = rsx_ptr();

    ////////////////////////////////////////

    // top points
    struct vec3 ta = mat44_mult_vec3(transform, make_vec3(aabb.min.x, aabb.min.z, aabb.max.y));
    struct vec3 tb = mat44_mult_vec3(transform, make_vec3(aabb.max.x, aabb.min.z, aabb.max.y));
    struct vec3 tc = mat44_mult_vec3(transform, make_vec3(aabb.max.x, aabb.max.z, aabb.max.y));
    struct vec3 td = mat44_mult_vec3(transform, make_vec3(aabb.min.x, aabb.max.z, aabb.max.y));

    // bottom points
    struct vec3 ba = mat44_mult_vec3(transform, make_vec3(aabb.min.x, aabb.min.z, aabb.min.y));
    struct vec3 bb = mat44_mult_vec3(transform, make_vec3(aabb.max.x, aabb.min.z, aabb.min.y));
    struct vec3 bc = mat44_mult_vec3(transform, make_vec3(aabb.max.x, aabb.max.z, aabb.min.y));
    struct vec3 bd = mat44_mult_vec3(transform, make_vec3(aabb.min.x, aabb.max.z, aabb.min.y));

    ////////////////////////////////////////

    f32 width = 1.0f;
    f32 lifetime = 0.0f;

    struct vec3 color = rsx_get_conf()->color.aabb;

    // top lines
    rsx_debug_add_line(ta, tb, color, width, lifetime, depth);
    rsx_debug_add_line(tb, tc, color, width, lifetime, depth);
    rsx_debug_add_line(tc, td, color, width, lifetime, depth);
    rsx_debug_add_line(td, ta, color, width, lifetime, depth);

    // bottom lines
    rsx_debug_add_line(ba, bb, color, width, lifetime, depth);
    rsx_debug_add_line(bb, bc, color, width, lifetime, depth);
    rsx_debug_add_line(bc, bd, color, width, lifetime, depth);
    rsx_debug_add_line(bd, ba, color, width, lifetime, depth);

    // connection lines
    rsx_debug_add_line(ba, ta, color, width, lifetime, depth);
    rsx_debug_add_line(bb, tb, color, width, lifetime, depth);
    rsx_debug_add_line(bc, tc, color, width, lifetime, depth);
    rsx_debug_add_line(bd, td, color, width, lifetime, depth);

error:
    return;
}
