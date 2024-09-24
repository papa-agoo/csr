////////////////////////////////////////////////////////////////////////////////////////////////////

#include "renderer_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_gizmos(struct renderer *renderer);

// static result_e _create_samplers(struct renderer_cache *cache);
// static result_e _create_textures(struct renderer_cache *cache);
// static result_e _create_shaders(struct renderer_cache *cache);
// static result_e _create_pipelines(struct renderer_cache *cache);

result_e renderer_init(struct renderer *renderer)
{
    check_ptr(renderer);

    check_result(_create_gizmos(renderer), "could not create gizmos");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void renderer_quit(struct renderer *renderer)
{
    check_ptr(renderer);

    struct renderer_cache *cache = &renderer->cache;

    // _destroy_gizmos(cache);

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

    struct {
        struct vec3 position;
        struct vec3 color;
    } vertex;

    u32 num_lines = 3;

    struct vector *vertices = vector_create(num_lines * 2, sizeof(vertex));
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

    struct mesh_gizmo *gizmo = &renderer->cache.gizmo.axes;
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

        result_e result = xgl_create_buffer(&info, gpu_vertices);
        check_result(result, "could not create gpu vertices");
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

    struct {
        struct vec3 position;
        struct vec3 color;
    } vertex;

    u32 num_lines = (2 * (size_qm + 1)) + 2;

    struct vector *vertices = vector_create(num_lines * 2, sizeof(vertex));
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

    struct mesh_gizmo *gizmo = &renderer->cache.gizmo.grid;
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
