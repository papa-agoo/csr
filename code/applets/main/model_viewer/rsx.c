////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet/aio.h> // FIXME remove screen dep., use render targets

#include "rsx_priv.h"
#include "rsx/rgpu_priv.h"
#include "rsx/rcpu_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_render_data();
static void _destroy_render_data();

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct rsx g_rsx = {0};

struct rsx* rsx_ptr()
{
    return &g_rsx;
}

result_e rsx_init(struct rsx_init_info *info)
{
    check_ptr(info);

    check_ptr(info->conf);
    check_ptr(info->arena);

    check_ptr(info->screen_rgpu);
    check_ptr(info->screen_rcpu);

    ////////////////////////////////////////

    struct rsx *rsx = rsx_ptr();

    rsx->conf = info->conf;
    rsx->arena = info->arena;

    // create gpu renderer
    rsx->screen.rgpu = info->screen_rgpu;
    {
        rsx->rgpu = rgpu_create();
        check_ptr(rsx->rgpu);
    }

    // create cpu renderer
    rsx->screen.rcpu = info->screen_rcpu;
    {
        rsx->rcpu = rcpu_create();
        check_ptr(rsx->rcpu);
    }

    ////////////////////////////////////////

    check_result(_create_render_data());

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_quit()
{
    _destroy_render_data();

    rcpu_destroy();
    rgpu_destroy();

error:
    return;
}

static void _build_frame_render_data(f64 dt)
{
    struct rsx_render_data *render_data = rsx_get_render_data();

    // sync cpu / gpu frame data
    struct rsx_uniform_buffer_frame *ubo = &render_data->frame.data;
    {
        void *gpu_ptr = xgl_map_buffer(ubo->gpu);

        memcpy(gpu_ptr, &ubo->cpu, sizeof(ubo->cpu));
        xgl_unmap_buffer(ubo->gpu);
    }

    // update pass data (suitable for gpu/cpu renderers)
    {
        // ...

        rsx_pass_gizmos_tick(rsx_pass_data_gizmos_ptr(), dt);
        // rsx_pass_debug_primitives_tick(rsx_pass_data_debug_primitives_ptr());

        // ...
    }

error:
    return;
}

void rsx_tick(f64 dt)
{
    // build render data for the current frame
    _build_frame_render_data(dt);

    ////////////////////////////////////////

    struct rsx *rsx = rsx_ptr();

    // update cpu screen aspect ratio (gpu/cpu screens share one view matrix)
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

error:
    return;
}

const struct rsx_conf* rsx_get_conf()
{
    return rsx_ptr()->conf;
}

struct rsx_render_data* rsx_get_render_data()
{
    return &rsx_ptr()->render_data;
}

static result_e _create_shader_resource_frame(struct rsx_shader_resource_frame *resource)
{
    check_ptr(resource);

    // uniform buffer
    struct rsx_uniform_buffer_frame *ubo = &resource->data;
    {
        shader_data_frame_init(&ubo->cpu);

        struct xgl_buffer_create_info info = {0};
        info.usage_flags = XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        info.byte_length = sizeof(ubo->cpu);
        info.data = &ubo->cpu;

        check_result(xgl_create_buffer(&info, &ubo->gpu));
    }

    // resource binding
    struct rsx_shader_resource_binding *binding = &resource->binding;
    {
        // copy layout info from cache
        struct rgpu_cache *cache = rsx_rgpu_cache_ptr();

        binding->ds_layout = cache->ds_layout.frame;
        binding->pipeline_layout = cache->pipeline_layout.main;

        check_result(xgl_create_descriptor_set(binding->ds_layout, &binding->ds));

        // update descriptor set
        struct xgl_buffer_descriptor descriptor = {0};
        descriptor.binding = 0;
        descriptor.buffer = ubo->gpu;

        struct xgl_descriptor_set_update_info update_info = {0};
        update_info.buffer_descriptors = &descriptor;
        update_info.buffer_descriptor_count = 1;

        check_result(xgl_update_descriptor_set(binding->ds, &update_info));
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _destroy_shader_resource_frame(struct rsx_shader_resource_frame *resource)
{
    check_ptr(resource);

    xgl_destroy_buffer(resource->data.gpu);
    xgl_destroy_descriptor_set(resource->binding.ds);

error:
    return;
}

static result_e _create_render_data()
{
    struct rsx_render_data *render_data = rsx_get_render_data();

    // create global shader data
    check_result(_create_shader_resource_frame(&render_data->frame));

    // create pass data
    check_result(rsx_pass_meshes_create(rsx_pass_data_meshes_ptr()));
    check_result(rsx_pass_gizmos_create(rsx_pass_data_gizmos_ptr()));
    check_result(rsx_pass_environment_create(rsx_pass_data_environment_ptr()));
    check_result(rsx_pass_debug_primitives_create(rsx_pass_data_debug_primitives_ptr()));

    // temp materials
    {
        struct rgpu_cache *cache_gpu = rsx_rgpu_cache_ptr();
        struct rcpu_cache *cache_cpu = rsx_rcpu_cache_ptr();

        struct rsx_material *debug_colors = &render_data->material.debug_colors;
        debug_colors->name = make_string("Debug Colors");
        debug_colors->pso.gpu = cache_gpu->pipeline.debug_colors;
        debug_colors->pso.cpu = cache_cpu->pipeline.debug_colors;
    }

    // ...

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _destroy_render_data()
{
    struct rsx_render_data *render_data = rsx_get_render_data();

    // destroy global frame data
    _destroy_shader_resource_frame(&render_data->frame);

    // destroy pass data
    rsx_pass_meshes_destroy(rsx_pass_data_meshes_ptr());
    rsx_pass_gizmos_destroy(rsx_pass_data_gizmos_ptr());
    rsx_pass_environment_destroy(rsx_pass_data_environment_ptr());
    rsx_pass_debug_primitives_destroy(rsx_pass_data_debug_primitives_ptr());
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass : meshes
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e rsx_pass_meshes_create(struct rsx_pass_meshes *pass_data)
{
    check_ptr(pass_data);

    pass_data->enabled = true;

    // private data
    struct rsx_pass_meshes_priv *priv = &pass_data->priv;
    {
        priv->meshes = vector_create(128, sizeof(struct rsx_mesh*));
        check_ptr(priv->meshes);
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_pass_meshes_destroy(struct rsx_pass_meshes *pass_data)
{
    check_ptr(pass_data);

    struct rsx_pass_meshes_priv *priv = &pass_data->priv;
    {
        vector_destroy(priv->meshes);
    }

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass : gizmos
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _calc_axes_gizmo_viewport(f32 *x, f32 *y, f32 *width, f32 *height)
{
    check_expr(x && y && width && height);
    check_expr(*width > 0 && *height > 0);

    // FIXME move factors to rsx config
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

static result_e _create_axes_gizmo(struct rsx_pass_gizmos_priv *priv)
{
    const struct rsx_conf *conf = rsx_get_conf();

    ////////////////////////////////////////

    struct vertex_1p1c vertices[] = {
        // x axis
        {.position = make_vec3_zero(), .color = conf->color.axis_x},
        {.position = make_vec3_x_axis(), .color = conf->color.axis_x},

        // y axis
        {.position = make_vec3_zero(), .color = conf->color.axis_y},
        {.position = make_vec3_y_axis(), .color = conf->color.axis_y},

        // z axis
        {.position = make_vec3_zero(), .color = conf->color.axis_z},
        {.position = make_vec3_z_axis(), .color = conf->color.axis_z},
    };

    ////////////////////////////////////////

    struct rsx_mesh_primitive_data data = {0};
    data.mode = RSX_PRIMITIVE_MODE_LINES;
    data.vertex_format = VERTEX_FORMAT_1P_1C;
    data.vertex_data = vertices;
    data.vertex_count = sizeof(vertices) / sizeof(struct vertex_1p1c);
    data.material = &priv->material.axes;

    struct rsx_mesh_create_info info = {0};
    info.name = make_string("gizmo.axes");
    info.arena = rsx_arena_ptr();
    info.primitives = &data;
    info.primitive_count = 1;

    priv->mesh.axes = rsx_mesh_create(&info);
    check_ptr(priv->mesh.axes);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_grid_gizmo(struct rsx_pass_gizmos_priv *priv, f32 size_qm)
{
    check_expr(size_qm >= 1);

    ////////////////////////////////////////

    struct vertex_1p1c vertex = {0};

    u32 num_lines = (2 * (size_qm + 1)) + 2;

    // FIXME scratch arena
    struct vector *vertices = vector_create(num_lines * 2, sizeof(struct vertex_1p1c));
    check_mem(vertices);

    ////////////////////////////////////////

    const struct rsx_conf *conf = rsx_get_conf();

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

    struct rsx_mesh_primitive_data data = {0};
    data.mode = RSX_PRIMITIVE_MODE_LINES;
    data.vertex_format = VERTEX_FORMAT_1P_1C;
    data.vertex_data = vector_data(vertices);
    data.vertex_count = vector_size(vertices);
    data.material = &priv->material.grid;

    struct rsx_mesh_create_info info = {0};
    info.name = make_string("gizmo.grid");
    info.arena = rsx_arena_ptr();
    info.primitives = &data;
    info.primitive_count = 1;

    priv->mesh.grid = rsx_mesh_create(&info);
    check_ptr(priv->mesh.grid);

    ////////////////////////////////////////

    vector_destroy(vertices);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e rsx_pass_gizmos_create(struct rsx_pass_gizmos *pass_data)
{
    check_ptr(pass_data);

    ////////////////////////////////////////

    pass_data->name = make_string("Pass Gizmos");

    pass_data->enabled = true;
    pass_data->draw_grid = true;
    pass_data->draw_orientation_axes = true;
    pass_data->draw_transform_handles = false;

    // private data
    struct rsx_pass_gizmos_priv *priv = &pass_data->priv;
    {
        // helper for the axes gizmo viewport (resoulution / dpi independent)
        priv->calc_axes_viewport = _calc_axes_gizmo_viewport;

        ////////////////////////////////////////

        struct rgpu_cache *cache_gpu = rsx_rgpu_cache_ptr();
        struct rcpu_cache *cache_cpu = rsx_rcpu_cache_ptr();

        // grid gizmo
        {
            // rsx_material material = rsx_material_create(...);

            struct rsx_material *material = &priv->material.grid;

            material->name = make_string("Grid Gizmo");
            material->pso.cpu = cache_cpu->pipeline.lines;
            material->pso.gpu = cache_gpu->pipeline.lines[RSX_PRIMITIVE_SIZE_NORMAL];

            check_result(_create_grid_gizmo(priv, 10));
        }

        // axes gizmo
        {
            // rsx_material material = rsx_material_create(...);

            struct rsx_material *material = &priv->material.axes;

            material->name = make_string("Axes Gizmo");
            material->pso.cpu = cache_cpu->pipeline.lines;
            material->pso.gpu = cache_gpu->pipeline.lines[RSX_PRIMITIVE_SIZE_THICK];

            check_result(_create_axes_gizmo(priv));
        }

        // imguizmo
        // ...
    }

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_pass_gizmos_destroy(struct rsx_pass_gizmos *pass_data)
{
    check_ptr(pass_data);

    struct rsx_pass_gizmos_priv *priv = &pass_data->priv;
    {
        rsx_mesh_destroy(priv->mesh.axes);
        rsx_mesh_destroy(priv->mesh.grid);
    }

error:
    return;
}

void rsx_pass_gizmos_tick(struct rsx_pass_gizmos *pass_data, f64 dt)
{
    check_ptr(pass_data);

    struct rsx_render_data *render_data = rsx_get_render_data();

    struct shader_data_frame *frame_data = &render_data->frame.data.cpu;

    struct rsx_pass_gizmos_priv *priv = &pass_data->priv;
    {
        struct rsx_uniform_buffer_object *ubo = &priv->mesh.axes->shader_data.data;
        ubo->cpu.use_object_mvp = true;

        // calc special mvp matrix which negates camera movement / scale and uses ortho proj.
        ubo->cpu.mtx_mvp = mat44_mult(frame_data->mtx_projection_ortho, frame_data->mtx_view);
        ubo->cpu.mtx_mvp = mat44_mult(ubo->cpu.mtx_mvp, mat44_translate(render_data->world_origin));

        void *ptr = xgl_map_buffer(ubo->gpu);
        memcpy(ptr, &ubo->cpu, sizeof(ubo->cpu));
        xgl_unmap_buffer(ubo->gpu);
    }

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass : environment
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e rsx_pass_environment_create(struct rsx_pass_environment *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_pass_environment_destroy(struct rsx_pass_environment *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass : debug primitives
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e rsx_pass_debug_primitives_create(struct rsx_pass_debug_primitives *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rsx_pass_debug_primitives_destroy(struct rsx_pass_debug_primitives *pass_data)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

error:
    return;
}

void rsx_pass_debug_primitives_tick(struct rsx_pass_debug_primitives *pass_data, f64 dt)
{
    check_ptr(pass_data);

    clog_warn("not impl. yet");

    // 1. collect primitives (points / lines) in a queue (high level stuff)
    //
    // 2. process queue
    //      - sort primitives by properties (type, depth, size, ...)
    //      - copy data to the vertex buffer
    //      - process sorted primitives
    //              - create rsx_material
    //                  - automated pipeline creation using the pipeline cache
    //              - create rsx_mesh_primitive
    //              - handle primitive lifetime
    //                  - remove expired primitive from the queue
    //                  - otherwise move the primitive to the front (for the next frame)
    //      - update rsx_mesh using the new data

    // struct rsx_render_data *render_data = rsx_get_render_data();

    // if (pass_data->draw_world_origin) {
    //     rsx_debug_add_point(render_data->world_origin, make_vec3(1, 1, 1), 3, 0, false);
    // }

error:
    return;
}
