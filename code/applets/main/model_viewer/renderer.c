////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet/aio.h>

#include "renderer_priv.h"
#include "renderer/rgpu_priv.h"
#include "renderer/rcpu_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_shader_data(struct renderer *renderer);
static void _destroy_shader_data(struct renderer *renderer);

static result_e _create_gizmos(struct renderer *renderer);
static void _destroy_gizmos(struct renderer *renderer);

static result_e _create_debug_primitives(struct renderer *renderer);
static void _reset_debug_primitives(struct renderer *renderer);
static void _destroy_debug_primitives(struct renderer *renderer);

result_e renderer_init(struct renderer *renderer)
{
    check_ptr(renderer);

    // init config
    struct renderer_conf *conf = &renderer->conf;
    renderer_conf_defaults(conf);

    ////////////////////////////////////////

    struct vec4 screen_clear_color = make_vec4_3_1(conf->color.background, 1.0);

    // create rgpu
    {
        struct screen_create_info create_info = {0};
        create_info.name = make_string("GPU Renderer");

        create_info.surface.type = SCREEN_SURFACE_TYPE_GPU;
        create_info.surface.viewport.width = 1280;
        create_info.surface.viewport.height = 720;
        create_info.surface.clear_values.color = screen_clear_color;
        create_info.surface.clear_values.depth = 1.0;
        create_info.is_suspended = !conf->enable_rgpu;

        renderer->screen.rgpu = aio_add_screen("rgpu", &create_info);
        check_ptr(renderer->screen.rgpu);

        renderer->rgpu = rgpu_create();
        check_ptr(renderer->rgpu);
    }

    // create rcpu
    {
        struct screen_create_info create_info = {0};
        create_info.name = make_string("CPU Renderer");

        create_info.surface.type = SCREEN_SURFACE_TYPE_CPU;
        create_info.surface.viewport.width = 640;
        create_info.surface.viewport.height = 360;
        create_info.surface.clear_values.color = screen_clear_color;
        create_info.surface.clear_values.depth = 1.0;
        create_info.keep_aspect_ratio = true;
        create_info.is_suspended = !conf->enable_rcpu;

        renderer->screen.rcpu = aio_add_screen("rcpu", &create_info);
        check_ptr(renderer->screen.rcpu);

        renderer->rcpu = rcpu_create();
        check_ptr(renderer->rcpu);
    }

    ////////////////////////////////////////

    check_result(_create_shader_data(renderer));
    check_result(_create_gizmos(renderer));
    check_result(_create_debug_primitives(renderer));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void renderer_quit(struct renderer *renderer)
{
    check_ptr(renderer);

    _destroy_debug_primitives(renderer);
    _destroy_gizmos(renderer);
    _destroy_shader_data(renderer);

    rcpu_destroy();
    rgpu_destroy();

error:
    return;
}

void renderer_tick(struct renderer *renderer)
{
    check_ptr(renderer);

    // build render structures (suitable for gpu/cpu renderers)
    {
        // ...
    }

    // update cpu screen aspect ratio
    screen_set_aspect_ratio(renderer->screen.rcpu, screen_get_aspect_ratio(renderer->screen.rgpu));

    // tick cpu renderer
    if (screen_begin(renderer->screen.rcpu, SCREEN_SURFACE_TYPE_CPU))
    {
        struct pixelbuffer *pb = screen_get_pixelbuffer(renderer->screen.rcpu);
        struct xgl_viewport vp = screen_get_viewport(renderer->screen.rcpu);

        rcpu_tick(renderer, pb, *(struct softgl_viewport*) &vp);

        screen_end();
    }

    // tick gpu renderer
    if (screen_begin(renderer->screen.rgpu, SCREEN_SURFACE_TYPE_GPU))
    {
        rgpu_tick(renderer, screen_get_viewport(renderer->screen.rgpu));

        screen_end();
    }

    // clear frame resources
    _reset_debug_primitives(renderer);

error:
    return;
}

void renderer_calc_axes_viewport(f32 *x, f32 *y, f32 *width, f32 *height)
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

static result_e _create_shader_data(struct renderer *renderer)
{
    check_ptr(renderer);

    struct rgpu_cache *cache = &renderer->rgpu->cache;
    struct renderer_shader_data *shader_data = &renderer->shader_data;

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
        data->mat_model = mat44_identity();
        data->mat_mvp = mat44_identity();
        data->use_object_mvp = false;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _destroy_shader_data(struct renderer *renderer)
{
    check_ptr(renderer);

    struct renderer_shader_data *shader_data = &renderer->shader_data;

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
static result_e _create_axes_gizmo(struct renderer *renderer)
{
    check_ptr(renderer);

    ////////////////////////////////////////

    struct vertex_1p1c vertex = {0};

    u32 num_lines = 3;

    struct vector *vertices = vector_create(num_lines * 2, sizeof(struct vertex_1p1c));
    check_mem(vertices);

    ////////////////////////////////////////

    // x axis
    vertex.color = renderer->conf.color.axis_x;

    vertex.position = make_vec3(0, 0, 0);
    vector_push_back(vertices, vertex);

    vertex.position = make_vec3_x_axis();
    vector_push_back(vertices, vertex);

    // y axis
    vertex.color = renderer->conf.color.axis_y;

    vertex.position = make_vec3(0, 0, 0);
    vector_push_back(vertices, vertex);

    vertex.position = make_vec3_y_axis();
    vector_push_back(vertices, vertex);

    // z axis
    vertex.color = renderer->conf.color.axis_z;

    vertex.position = make_vec3(0, 0, 0);
    vector_push_back(vertices, vertex);

    vertex.position = make_vec3_z_axis();
    vector_push_back(vertices, vertex);

    ////////////////////////////////////////

    struct mesh_gizmo *gizmo = &renderer->gizmo.axes;
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

static result_e _create_grid_gizmo(struct renderer *renderer, f32 size_qm)
{
    check_ptr(renderer);
    check_expr(size_qm >= 1);

    ////////////////////////////////////////

    struct vertex_1p1c vertex = {0};

    u32 num_lines = (2 * (size_qm + 1)) + 2;

    struct vector *vertices = vector_create(num_lines * 2, sizeof(struct vertex_1p1c));
    check_mem(vertices);

    ////////////////////////////////////////

    f32 e = size_qm / 2.0;
    f32 step_size = e / e;

    // generate grid lines
    for (f32 i = -e; i <= e; i += step_size)
    {
        vertex.color = renderer->conf.color.grid;

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
        vertex.color = renderer->conf.color.axis_x;

        vertex.position = make_vec3(0, 0, 0);
        vector_push_back(vertices, vertex);

        vertex.position = make_vec3(e, 0, 0);
        vector_push_back(vertices, vertex);
    }

    // colored z axis from origin to e
    {
        vertex.color = renderer->conf.color.axis_z;

        vertex.position = make_vec3(0, 0, 0);
        vector_push_back(vertices, vertex);

        vertex.position = make_vec3(0, 0, e);
        vector_push_back(vertices, vertex);
    }

    ////////////////////////////////////////

    // struct aabb aabb = make_aabb(make_vec3(-e, 0, -e), make_vec3(e, 0, e));

    struct mesh_gizmo *gizmo = &renderer->gizmo.grid;
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

static result_e _create_gizmos(struct renderer *renderer)
{
    check_ptr(renderer);

    check_result(_create_axes_gizmo(renderer));
    check_result(_create_grid_gizmo(renderer, 10));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _destroy_gizmos(struct renderer *renderer)
{
    check_ptr(renderer);

    // axes
    {
        struct mesh_gizmo *gizmo = &renderer->gizmo.axes;

        vector_destroy(gizmo->buffer.vertices.cpu);
        xgl_destroy_buffer(gizmo->buffer.vertices.gpu);
    }

    // grid
    {
        struct mesh_gizmo *gizmo = &renderer->gizmo.grid;

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
static result_e _create_debug_primitives_buffer(struct renderer *renderer, struct mesh_buffer *buffer, u32 capacity, u64 element_size)
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

static result_e _create_debug_primitives(struct renderer *renderer)
{
    check_ptr(renderer);

    u32 vertex_format = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT;
    u32 vertex_stride = sizeof(struct vertex_1p1c);

    u32 max_points = 1024;
    u32 max_lines = max_points * 2;

    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        check_result(_create_debug_primitives_buffer(renderer, &renderer->debug_draw.points[i], max_points, vertex_stride));
        check_result(_create_debug_primitives_buffer(renderer, &renderer->debug_draw.points_no_depth[i], max_points, vertex_stride));

        check_result(_create_debug_primitives_buffer(renderer, &renderer->debug_draw.lines[i], max_lines, vertex_stride));
        check_result(_create_debug_primitives_buffer(renderer, &renderer->debug_draw.lines_no_depth[i], max_lines, vertex_stride));
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _reset_debug_primitives(struct renderer *renderer)
{
    check_ptr(renderer);

    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        vector_clear(renderer->debug_draw.points[i].vertices.cpu);
        vector_clear(renderer->debug_draw.points_no_depth[i].vertices.cpu);

        vector_clear(renderer->debug_draw.lines[i].vertices.cpu);
        vector_clear(renderer->debug_draw.lines_no_depth[i].vertices.cpu);
    }

error:
    return;
}

static void _destroy_debug_primitives(struct renderer *renderer)
{
    check_ptr(renderer);

    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        _destroy_debug_primitives_buffer(&renderer->debug_draw.points[i]);
        _destroy_debug_primitives_buffer(&renderer->debug_draw.points_no_depth[i]);

        _destroy_debug_primitives_buffer(&renderer->debug_draw.lines[i]);
        _destroy_debug_primitives_buffer(&renderer->debug_draw.lines_no_depth[i]);
    }

error:
    return;
}

void renderer_add_point(struct renderer *renderer, struct vec3 p, struct vec3 color, f32 size, f32 lifetime, bool depth)
{
    check_ptr(renderer);

    // FIXME lifetime

    struct vertex_1p1c v = {.position = p, .color = color};

    u32 size_idx = clamp(size, 1, PRIMITIVE_SIZE_MAX) - 1;

    struct mesh_buffer *buffer = (depth) ? &renderer->debug_draw.points[size_idx] : &renderer->debug_draw.points_no_depth[size_idx];

    vector_push_back(buffer->vertices.cpu, v);

error:
    return;
}

void renderer_add_line(struct renderer *renderer, struct vec3 a, struct vec3 b, struct vec3 color, f32 width, f32 lifetime, bool depth)
{
    check_ptr(renderer);

    // FIXME lifetime

    struct vertex_1p1c va = {.position = a, .color = color};
    struct vertex_1p1c vb = {.position = b, .color = color};

    u32 size_idx = clamp(width, 1, PRIMITIVE_SIZE_MAX) - 1;

    struct mesh_buffer *buffer = (depth) ? &renderer->debug_draw.lines[size_idx] : &renderer->debug_draw.lines_no_depth[size_idx];

    vector_push_back(buffer->vertices.cpu, va);
    vector_push_back(buffer->vertices.cpu, vb);

error:
    return;
}

void renderer_add_axes(struct renderer *renderer, struct mat44 transform, bool depth)
{
    check_ptr(renderer);

    ////////////////////////////////////////

    struct vec3 origin  = mat44_mult_vec3(transform, make_vec3_zero());
    struct vec3 basis_x = mat44_mult_vec3(transform, make_vec3_x_axis());
    struct vec3 basis_y = mat44_mult_vec3(transform, make_vec3_y_axis());
    struct vec3 basis_z = mat44_mult_vec3(transform, make_vec3_z_axis());

    ////////////////////////////////////////

    f32 width = 2.0f;
    f32 lifetime = 0.0f;

    struct renderer_conf *conf = &renderer->conf;

    renderer_add_line(renderer, origin, basis_x, conf->color.axis_x, width, lifetime, depth);
    renderer_add_line(renderer, origin, basis_y, conf->color.axis_y, width, lifetime, depth);
    renderer_add_line(renderer, origin, basis_z, conf->color.axis_z, width, lifetime, depth);

error:
    return;
}

void renderer_add_aabb(struct renderer *renderer, struct mat44 transform, struct aabb aabb, bool depth)
{
    check_ptr(renderer);

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

    struct vec3 color = renderer->conf.color.aabb;

    // top lines
    renderer_add_line(renderer, ta, tb, color, width, lifetime, depth);
    renderer_add_line(renderer, tb, tc, color, width, lifetime, depth);
    renderer_add_line(renderer, tc, td, color, width, lifetime, depth);
    renderer_add_line(renderer, td, ta, color, width, lifetime, depth);

    // bottom lines
    renderer_add_line(renderer, ba, bb, color, width, lifetime, depth);
    renderer_add_line(renderer, bb, bc, color, width, lifetime, depth);
    renderer_add_line(renderer, bc, bd, color, width, lifetime, depth);
    renderer_add_line(renderer, bd, ba, color, width, lifetime, depth);

    // connection lines
    renderer_add_line(renderer, ba, ta, color, width, lifetime, depth);
    renderer_add_line(renderer, bb, tb, color, width, lifetime, depth);
    renderer_add_line(renderer, bc, tc, color, width, lifetime, depth);
    renderer_add_line(renderer, bd, td, color, width, lifetime, depth);

error:
    return;
}
