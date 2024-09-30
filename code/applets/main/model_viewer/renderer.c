////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet/aio.h>

#include "renderer_priv.h"
#include "renderer/rgpu_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_shader_data(struct renderer *renderer);
static result_e _create_gizmos(struct renderer *renderer);

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
        create_info.name = "GPU Renderer";

        create_info.surface.viewport.width = 1280;
        create_info.surface.viewport.height = 720;
        create_info.surface.clear_values.color = screen_clear_color;
        create_info.surface.clear_values.depth = 1.0;

        renderer->screen.rgpu = aio_add_screen("rgpu", &create_info);
        check_ptr(renderer->screen.rgpu);

        renderer->rgpu = rgpu_create();
        check_ptr(renderer->rgpu);
    }

    // create rcpu
    {
        struct screen_create_info create_info = {0};
        create_info.name = "CPU Renderer";

        create_info.surface.type = SCREEN_SURFACE_TYPE_CPU;
        create_info.surface.viewport.width = 640;
        create_info.surface.viewport.height = 360;
        create_info.surface.clear_values.color = screen_clear_color;
        create_info.surface.clear_values.depth = 1.0;

        renderer->screen.rcpu = aio_add_screen("rcpu", &create_info);
        check_ptr(renderer->screen.rcpu);

        renderer->rcpu = rcpu_create();
        // check_ptr(renderer->rcpu);
    }

    ////////////////////////////////////////

    check_result(_create_shader_data(renderer), "could not create shader data");
    check_result(_create_gizmos(renderer), "could not create gizmos");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void renderer_quit(struct renderer *renderer)
{
    check_ptr(renderer);

    // rcpu_destroy();
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

    // gpu renderer
    if (screen_begin(renderer->screen.rgpu, SCREEN_SURFACE_TYPE_GPU))
    {
        rgpu_tick(renderer, screen_get_viewport(renderer->screen.rgpu));

        screen_end();
    }

    // cpu renderer
    if (screen_begin(renderer->screen.rcpu, SCREEN_SURFACE_TYPE_CPU))
    {
        // rcpu_tick(renderer);

        screen_end();
    }

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

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// gizmos
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _create_axes_gizmo(struct renderer *renderer)
{
    check_ptr(renderer);

    ////////////////////////////////////////

    struct vertex_1p1c vertex;

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

    gizmo->buffer.vertex_format = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT;
    gizmo->buffer.vertex_stride = sizeof(vertex);

    gizmo->primitive.buffer = &gizmo->buffer;
    gizmo->primitive.vertices.count = vector_size(vertices);

    gizmo->buffer.cpu.vertices = vertices;

    xgl_buffer *gpu_vertices = &gizmo->buffer.gpu.vertices;
    {
        struct xgl_buffer_create_info info = {0};
        info.byte_length = vector_byte_length(vertices);
        info.data = vector_data(vertices);
        info.usage_flags = XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        check_result(xgl_create_buffer(&info, gpu_vertices), "could not create gpu vertices");
    }

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_grid_gizmo(struct renderer *renderer, f32 size_qm)
{
    check_ptr(renderer);
    check_expr(size_qm >= 1);

    ////////////////////////////////////////

    struct vertex_1p1c vertex;

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

    gizmo->buffer.vertex_format = VERTEX_FORMAT_POSITION_BIT | VERTEX_FORMAT_COLOR_BIT;
    gizmo->buffer.vertex_stride = sizeof(vertex);

    gizmo->primitive.buffer = &gizmo->buffer;
    gizmo->primitive.vertices.count = vector_size(vertices);

    gizmo->buffer.cpu.vertices = vertices;

    xgl_buffer *gpu_vertices = &gizmo->buffer.gpu.vertices;
    {
        struct xgl_buffer_create_info info = {0};
        info.byte_length = vector_byte_length(vertices);
        info.data = vector_data(vertices);
        info.usage_flags = XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        result_e result = xgl_create_buffer(&info, gpu_vertices);
        check_result(result, "could not create gpu vertices");
    }

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_gizmos(struct renderer *renderer)
{
    check_ptr(renderer);

    check_result(_create_axes_gizmo(renderer), "could not create axes gizmo");
    check_result(_create_grid_gizmo(renderer, 100), "could not create grid gizmo");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// materials
////////////////////////////////////////////////////////////////////////////////////////////////////

// ...


////////////////////////////////////////////////////////////////////////////////////////////////////
// debug draw
////////////////////////////////////////////////////////////////////////////////////////////////////

// ...
