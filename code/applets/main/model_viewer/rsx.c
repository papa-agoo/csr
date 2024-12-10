////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet/aio.h>

#include "rsx_priv.h"
#include "rsx/rgpu_priv.h"
#include "rsx/rcpu_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_shader_data();
static void _destroy_shader_data();

static result_e _create_gizmos();
static void _destroy_gizmos();

static result_e _create_debug_primitives();
static void _reset_debug_primitives();
static void _destroy_debug_primitives();

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct rsx g_rsx = {0};

struct rsx* rsx_ptr()
{
    return &g_rsx;
}

struct application* application_ptr();

result_e rsx_init(struct rsx_init_info *info)
{
    check_ptr(info);
    check_ptr(info->conf);
    check_ptr(info->screen_rgpu);
    check_ptr(info->screen_rcpu);

    struct rsx *rsx = rsx_ptr();

    rsx->conf = info->conf;

    rsx->screen.rgpu = info->screen_rgpu;
    rsx->rgpu = rgpu_create();
    check_ptr(rsx->rgpu);

    rsx->screen.rcpu = info->screen_rcpu;
    rsx->rcpu = rcpu_create();
    check_ptr(rsx->rcpu);

    ////////////////////////////////////////

    check_result(_create_shader_data());
    check_result(_create_gizmos());
    check_result(_create_debug_primitives());

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_quit()
{
    _destroy_debug_primitives();
    _destroy_gizmos();
    _destroy_shader_data();

    rcpu_destroy();
    rgpu_destroy();

error:
    return;
}

void rsx_tick()
{
    struct rsx *rsx = rsx_ptr();

    // build render structures (suitable for gpu/cpu renderers)
    {
        // ...
    }

    // update cpu screen aspect ratio
    screen_set_aspect_ratio(rsx->screen.rcpu, screen_get_aspect_ratio(rsx->screen.rgpu));

    // tick cpu renderer
    if (screen_begin(rsx->screen.rcpu, SCREEN_SURFACE_TYPE_CPU))
    {
        struct pixelbuffer *pb = screen_get_pixelbuffer(rsx->screen.rcpu);
        struct xgl_viewport vp = screen_get_viewport(rsx->screen.rcpu);

        rcpu_tick(pb, *(struct softgl_viewport*) &vp);

        screen_end();
    }

    // tick gpu renderer
    if (screen_begin(rsx->screen.rgpu, SCREEN_SURFACE_TYPE_GPU))
    {
        rgpu_tick(screen_get_viewport(rsx->screen.rgpu));

        screen_end();
    }

    // clear frame resources
    _reset_debug_primitives();

error:
    return;
}

const struct rsx_conf* rsx_get_conf()
{
    return rsx_ptr()->conf;
}

void rsx_calc_axes_viewport(f32 *x, f32 *y, f32 *width, f32 *height)
{
    check_expr(x && y && width && height);
    check_expr(*width > 0 && *height > 0);

    // FIXME move factors to renderer config
    f32 scale_vp = 0.05;
    f32 scale_margin = 0.20;

    struct vec2 size = {.w = *width * scale_vp, .h = *height * scale_vp};

    f32 size_xy = size.h;
    f32 size_xy_offset = (size.w - size.h) * 0.5;
    f32 margin_xy = size.h * scale_margin;

    *x = *width - size_xy - margin_xy - size_xy_offset;
    *y = *height - size_xy - margin_xy;

    *width = size.w;
    *height = size.h;

error:
    return;
}

static result_e _create_shader_data()
{
    struct rsx *rsx = rsx_ptr();

    struct rgpu_cache *cache = &rsx->rgpu->cache;
    struct rsx_shader_data *shader_data = &rsx->shader_data;

    // frame
    {
        // create buffer
        struct xgl_buffer_create_info info = {0};
        info.usage_flags = XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        info.byte_length = sizeof(struct shader_data_frame);
        info.data = &shader_data->frame.buffer.cpu;

        check_result(xgl_create_buffer(&info, &shader_data->frame.buffer.gpu));

        // create descriptor set
        check_result(xgl_create_descriptor_set(cache->ds_layout.frame, &shader_data->frame.ds));

        // update descriptor set
        struct xgl_buffer_descriptor descriptor = {0};
        descriptor.binding = 0;
        descriptor.buffer = shader_data->frame.buffer.gpu;

        struct xgl_descriptor_set_update_info update_info = {0};
        update_info.buffer_descriptors = &descriptor;
        update_info.buffer_descriptor_count = 1;

        check_result(xgl_update_descriptor_set(shader_data->frame.ds, &update_info));
    }

    // pass main
    {
        // create buffer
        struct xgl_buffer_create_info info = {0};
        info.usage_flags = XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        info.byte_length = sizeof(struct shader_data_pass);
        info.data = &shader_data->pass_main.buffer.cpu;

        check_result(xgl_create_buffer(&info, &shader_data->pass_main.buffer.gpu));

        // create descriptor set
        check_result(xgl_create_descriptor_set(cache->ds_layout.pass, &shader_data->pass_main.ds));

        // update descriptor set
        struct xgl_buffer_descriptor descriptor = {0};
        descriptor.binding = 0;
        descriptor.buffer = shader_data->pass_main.buffer.gpu;

        struct xgl_descriptor_set_update_info update_info = {0};
        update_info.buffer_descriptors = &descriptor;
        update_info.buffer_descriptor_count = 1;

        check_result(xgl_update_descriptor_set(shader_data->pass_main.ds, &update_info));
    }

    // pass environment
    {
        // ...
    }

    // object
    {
        // create buffer
        struct xgl_buffer_create_info info = {0};
        info.usage_flags = XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        info.byte_length = sizeof(struct shader_data_object);
        info.data = &shader_data->object.buffer.cpu;

        check_result(xgl_create_buffer(&info, &shader_data->object.buffer.gpu));

        // create descriptor set
        check_result(xgl_create_descriptor_set(cache->ds_layout.object, &shader_data->object.ds));

        // update descriptor set
        struct xgl_buffer_descriptor descriptor = {0};
        descriptor.binding = 0;
        descriptor.buffer = shader_data->object.buffer.gpu;

        struct xgl_descriptor_set_update_info update_info = {0};
        update_info.buffer_descriptors = &descriptor;
        update_info.buffer_descriptor_count = 1;

        check_result(xgl_update_descriptor_set(shader_data->object.ds, &update_info));

        // set default values
        struct shader_data_object *data = info.data;
        data->mtx_model = mat44_identity();
        data->mtx_mvp = mat44_identity();
        data->use_object_mvp = false;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _destroy_shader_data()
{
    struct rsx *rsx = rsx_ptr();

    struct rsx_shader_data *shader_data = &rsx->shader_data;

    xgl_destroy_buffer(shader_data->frame.buffer.gpu);
    xgl_destroy_descriptor_set(shader_data->frame.ds);

    xgl_destroy_buffer(shader_data->pass_main.buffer.gpu);
    xgl_destroy_descriptor_set(shader_data->pass_main.ds);

    xgl_destroy_buffer(shader_data->object.buffer.gpu);
    xgl_destroy_descriptor_set(shader_data->object.ds);

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// gizmos
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _create_axes_gizmo()
{
    const struct rsx_conf *conf = rsx_get_conf();

    ////////////////////////////////////////

    struct vertex_1p1c vertex = {0};

    u32 num_lines = 3;

    // FIXME scratch arena
    struct vector *vertices = vector_create(num_lines * 2, sizeof(struct vertex_1p1c));
    check_mem(vertices);

    ////////////////////////////////////////

    // x axis
    vertex.color = conf->color.axis_x;

    vertex.position = make_vec3_zero();
    vector_push_back(vertices, vertex);

    vertex.position = make_vec3_x_axis();
    vector_push_back(vertices, vertex);

    // y axis
    vertex.color = conf->color.axis_y;

    vertex.position = make_vec3_zero();
    vector_push_back(vertices, vertex);

    vertex.position = make_vec3_y_axis();
    vector_push_back(vertices, vertex);

    // z axis
    vertex.color = conf->color.axis_z;

    vertex.position = make_vec3_zero();
    vector_push_back(vertices, vertex);

    vertex.position = make_vec3_z_axis();
    vector_push_back(vertices, vertex);

    ////////////////////////////////////////

    struct mesh_gizmo *gizmo = &rsx_ptr()->gizmo.axes;
    gizmo->name = make_string("gizmo.axes");

    gizmo->primitive.vertex_format = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT;
    gizmo->primitive.vertex_stride = sizeof(vertex);

    gizmo->primitive.buffer = &gizmo->buffer;
    gizmo->primitive.vertices.count = vector_size(vertices);

    gizmo->buffer.vertices.cpu = vertices;

    xgl_buffer *gpu_vertices = &gizmo->buffer.vertices.gpu;
    {
        struct xgl_buffer_create_info info = {0};
        info.byte_length = vector_byte_length(vertices);
        info.data = vector_data(vertices);
        info.usage_flags = XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        check_result(xgl_create_buffer(&info, gpu_vertices));
    }

    ////////////////////////////////////////

    // 1. create mesh_primitive_data (vertex format, vertices, indices)
    //  - vertex format
    //  - vertices
    //  - indices
    //
    // 2. create mesh (vertex format, vertices, indices)
    //  - mesh_add_primitive(data, material)

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_grid_gizmo(f32 size_qm)
{
    check_expr(size_qm >= 1);

    const struct rsx_conf *conf = rsx_get_conf();

    ////////////////////////////////////////

    struct vertex_1p1c vertex = {0};

    u32 num_lines = (2 * (size_qm + 1)) + 2;

    // FIXME scratch arena
    struct vector *vertices = vector_create(num_lines * 2, sizeof(struct vertex_1p1c));
    check_mem(vertices);

    ////////////////////////////////////////

    f32 e = size_qm / 2.0;
    f32 step_size = e / e;

    // generate grid lines
    for (f32 i = -e; i <= e; i += step_size)
    {
        vertex.color = conf->color.grid;

        // along x axis
        {
            // p1
            vertex.position = make_vec3(-e, 0, i);
            vector_push_back(vertices, vertex);

            // p2
            vertex.position = (i == 0) ? make_vec3(0, 0, 0) : make_vec3(e, 0, i);
            vector_push_back(vertices, vertex);
        }

        // along z axis
        {
            // p1
            vertex.position = make_vec3(i, 0, -e);
            vector_push_back(vertices, vertex);

            // p2
            vertex.position = (i == 0) ? make_vec3(0, 0, 0) : make_vec3(i, 0, e);
            vector_push_back(vertices, vertex);
        }
    }

    // colored x axis from origin to e
    {
        vertex.color = conf->color.axis_x;

        vertex.position = make_vec3(0, 0, 0);
        vector_push_back(vertices, vertex);

        vertex.position = make_vec3(e, 0, 0);
        vector_push_back(vertices, vertex);
    }

    // colored z axis from origin to e
    {
        vertex.color = conf->color.axis_z;

        vertex.position = make_vec3(0, 0, 0);
        vector_push_back(vertices, vertex);

        vertex.position = make_vec3(0, 0, e);
        vector_push_back(vertices, vertex);
    }

    ////////////////////////////////////////

    // struct aabb aabb = make_aabb(make_vec3(-e, 0, -e), make_vec3(e, 0, e));

    struct mesh_gizmo *gizmo = &rsx_ptr()->gizmo.grid;
    gizmo->name = make_string("gizmo.grid");

    gizmo->primitive.vertex_format = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT;
    gizmo->primitive.vertex_stride = sizeof(vertex);

    gizmo->primitive.buffer = &gizmo->buffer;
    gizmo->primitive.vertices.count = vector_size(vertices);

    gizmo->buffer.vertices.cpu = vertices;

    xgl_buffer *gpu_vertices = &gizmo->buffer.vertices.gpu;
    {
        struct xgl_buffer_create_info info = {0};
        info.byte_length = vector_byte_length(vertices);
        info.data = vector_data(vertices);
        info.usage_flags = XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        check_result(xgl_create_buffer(&info, gpu_vertices));
    }

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_gizmos()
{
    check_result(_create_axes_gizmo());
    check_result(_create_grid_gizmo(10));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _destroy_gizmos()
{
    struct rsx *rsx = rsx_ptr();

    // axes
    {
        struct mesh_gizmo *gizmo = &rsx->gizmo.axes;

        vector_destroy(gizmo->buffer.vertices.cpu);
        xgl_destroy_buffer(gizmo->buffer.vertices.gpu);
    }

    // grid
    {
        struct mesh_gizmo *gizmo = &rsx->gizmo.grid;

        vector_destroy(gizmo->buffer.vertices.cpu);
        xgl_destroy_buffer(gizmo->buffer.vertices.gpu);
    }

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// materials
////////////////////////////////////////////////////////////////////////////////////////////////////

// ...


////////////////////////////////////////////////////////////////////////////////////////////////////
// debug primitives
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _create_debug_primitives_buffer(struct mesh_buffer *buffer, u32 capacity, u64 element_size)
{
    // buffer->vertex_format = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT;
    // buffer->vertex_stride = sizeof(struct vertex_1p1c);

    // cpu storage
    buffer->vertices.cpu = vector_create(capacity, element_size);

    // gpu storage
    struct xgl_buffer_create_info info = {0};
    info.byte_length = vector_capacity_byte_length(buffer->vertices.cpu);
    info.usage_flags = XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    return xgl_create_buffer(&info, &buffer->vertices.gpu);

error:
    return RC_FAILURE;
}

static void _destroy_debug_primitives_buffer(struct mesh_buffer *buffer)
{
    vector_destroy(buffer->vertices.cpu);
    xgl_destroy_buffer(buffer->vertices.gpu);
}

static result_e _create_debug_primitives()
{
    struct rsx *rsx = rsx_ptr();

    u32 vertex_format = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT;
    u32 vertex_stride = sizeof(struct vertex_1p1c);

    u32 max_points = 1024;
    u32 max_lines = max_points * 2;

    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        check_result(_create_debug_primitives_buffer(&rsx->debug_draw.points[i], max_points, vertex_stride));
        check_result(_create_debug_primitives_buffer(&rsx->debug_draw.points_no_depth[i], max_points, vertex_stride));

        check_result(_create_debug_primitives_buffer(&rsx->debug_draw.lines[i], max_lines, vertex_stride));
        check_result(_create_debug_primitives_buffer(&rsx->debug_draw.lines_no_depth[i], max_lines, vertex_stride));
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _reset_debug_primitives()
{
    struct rsx *rsx = rsx_ptr();

    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        vector_clear(rsx->debug_draw.points[i].vertices.cpu);
        vector_clear(rsx->debug_draw.points_no_depth[i].vertices.cpu);

        vector_clear(rsx->debug_draw.lines[i].vertices.cpu);
        vector_clear(rsx->debug_draw.lines_no_depth[i].vertices.cpu);
    }

error:
    return;
}

static void _destroy_debug_primitives()
{
    struct rsx *rsx = rsx_ptr();

    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        _destroy_debug_primitives_buffer(&rsx->debug_draw.points[i]);
        _destroy_debug_primitives_buffer(&rsx->debug_draw.points_no_depth[i]);

        _destroy_debug_primitives_buffer(&rsx->debug_draw.lines[i]);
        _destroy_debug_primitives_buffer(&rsx->debug_draw.lines_no_depth[i]);
    }

error:
    return;
}

void rsx_add_point(struct vec3 p, struct vec3 color, f32 size, f32 lifetime, bool depth)
{
    struct rsx *rsx = rsx_ptr();

    // FIXME lifetime

    struct vertex_1p1c v = {.position = p, .color = color};

    u32 size_idx = clamp(size, 1, PRIMITIVE_SIZE_MAX) - 1;

    struct mesh_buffer *buffer = (depth) ? &rsx->debug_draw.points[size_idx] : &rsx->debug_draw.points_no_depth[size_idx];

    vector_push_back(buffer->vertices.cpu, v);

error:
    return;
}

void rsx_add_line(struct vec3 a, struct vec3 b, struct vec3 color, f32 width, f32 lifetime, bool depth)
{
    struct rsx *rsx = rsx_ptr();

    // FIXME lifetime

    struct vertex_1p1c va = {.position = a, .color = color};
    struct vertex_1p1c vb = {.position = b, .color = color};

    u32 size_idx = clamp(width, 1, PRIMITIVE_SIZE_MAX) - 1;

    struct mesh_buffer *buffer = (depth) ? &rsx->debug_draw.lines[size_idx] : &rsx->debug_draw.lines_no_depth[size_idx];

    vector_push_back(buffer->vertices.cpu, va);
    vector_push_back(buffer->vertices.cpu, vb);

error:
    return;
}

void rsx_add_axes(struct mat44 transform, bool depth)
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

    rsx_add_line(origin, basis_x, conf->color.axis_x, width, lifetime, depth);
    rsx_add_line(origin, basis_y, conf->color.axis_y, width, lifetime, depth);
    rsx_add_line(origin, basis_z, conf->color.axis_z, width, lifetime, depth);

error:
    return;
}

void rsx_add_aabb(struct mat44 transform, struct aabb aabb, bool depth)
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
    rsx_add_line(ta, tb, color, width, lifetime, depth);
    rsx_add_line(tb, tc, color, width, lifetime, depth);
    rsx_add_line(tc, td, color, width, lifetime, depth);
    rsx_add_line(td, ta, color, width, lifetime, depth);

    // bottom lines
    rsx_add_line(ba, bb, color, width, lifetime, depth);
    rsx_add_line(bb, bc, color, width, lifetime, depth);
    rsx_add_line(bc, bd, color, width, lifetime, depth);
    rsx_add_line(bd, ba, color, width, lifetime, depth);

    // connection lines
    rsx_add_line(ba, ta, color, width, lifetime, depth);
    rsx_add_line(bb, tb, color, width, lifetime, depth);
    rsx_add_line(bc, tc, color, width, lifetime, depth);
    rsx_add_line(bd, td, color, width, lifetime, depth);

error:
    return;
}
