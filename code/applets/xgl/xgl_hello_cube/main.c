////////////////////////////////////////////////////////////////////////////////////////////////////

#include "main.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

bool g_animate_cube = true;
bool g_update_shader_bindings = true;

extern struct my_cube g_cube;

////////////////////////////////////////////////////////////////////////////////////////////////////

void update_shader_bindings()
{
    if (g_update_shader_bindings)
    {
        struct xgl_texture_descriptor cube_texture = {0};
        cube_texture.binding = 0;
        cube_texture.sampler = g_cube.material.sampler;
        cube_texture.texture = g_cube.material.texture;

        struct xgl_descriptor_set_update_info info = {0};
        info.texture_descriptors = &cube_texture;
        info.texture_descriptor_count = 1;

        xgl_update_descriptor_set(g_cube.descriptor_set, &info);
    }

    g_update_shader_bindings = false;
}

static void update_shader_params()
{
    // calc screen aspect ratio
    struct vec2 size = screen_get_size(aio_get_screen("main"));
    f32 aspect = (size.w / size.h);

    // calc rotation
    f32 deg = aio_time_elapsed() * 16;
    struct vec3 rotation = (g_animate_cube) ? make_vec3(deg * 0.5, deg * 0.75, deg) : make_vec3_zero();

    // build mvp matrix
    struct mat44 model = mat44_compose(make_vec3(0.0f, 0.0f, -5.0f), rotation, make_vec3_one());
    struct mat44 view = mat44_identity();
    struct mat44 proj = mat44_fovy(45, aspect, 0.1, 10.0);

    g_cube.material.shader_params.cpu.mvp = mat44_mult(proj, mat44_mult(view, model));

    ////////////////////////////////////////

    // copy to gpu
    void *ubo_data = xgl_map_buffer(g_cube.material.shader_params.gpu);
    check_ptr(ubo_data);

    memcpy(ubo_data, &g_cube.material.shader_params.cpu, sizeof(struct cube_data));
    xgl_unmap_buffer(g_cube.material.shader_params.gpu);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct string _plugin_get_name()
{
    return make_string("XGL Hello Cube");
}

static struct string _plugin_get_description()
{
    return make_string("Rendering a rotating Cube using XGL");
}

static struct version _plugin_get_version()
{
    return make_version(1, 0, 0);
}

static result_e _plugin_startup()
{
    // add screen
    struct screen_create_info create_info = {0};
    create_info.name = "XGL Cube";

    create_info.surface.type = SCREEN_SURFACE_TYPE_GPU;
    create_info.surface.clear_values.color = make_vec4(0.22, 0.22, 0.22, 1.0);

    struct screen *screen = aio_add_screen("main", &create_info);
    check_ptr(screen);

    // create resources
    check_expr(create_resources() == RC_SUCCESS);

    // create ui
    register_ui();

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _plugin_shutdown()
{
    destroy_resources();
}

static void _plugin_tick()
{
    update_shader_bindings();
    update_shader_params();

    struct screen *screen = aio_get_screen("main");

    if (screen_begin(screen, SCREEN_SURFACE_TYPE_GPU))
    {
        xgl_buffer vertex_buffers[] = {
            g_cube.geometry.vertex_buffer.positions,
            g_cube.geometry.vertex_buffer.tex_coords
        };

        u32 vb_offsets[] = {0, 0};
        u32 vb_strides[] = {sizeof(struct vec3), sizeof(struct vec2)};
        u32 vb_count = COUNT_OF(vertex_buffers);

        xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, g_cube.material.effect.pipeline);
        xgl_bind_descriptor_set(g_cube.material.effect.pipeline_layout, 0, g_cube.descriptor_set);

        xgl_bind_index_buffer(g_cube.geometry.index_buffer.indices);
        xgl_bind_vertex_buffers(0, vb_count, vertex_buffers, vb_offsets, vb_strides);

        xgl_draw_indexed(0, g_cube.geometry.index_buffer.index_count);

        screen_end();
    }
}

void applet_plugin_export(struct applet_plugin* plugin)
{
    plugin->get_name = _plugin_get_name;
    plugin->get_description = _plugin_get_description;
    plugin->get_version = _plugin_get_version;

    plugin->startup = _plugin_startup;
    plugin->shutdown = _plugin_shutdown;
    plugin->tick = _plugin_tick;
}
