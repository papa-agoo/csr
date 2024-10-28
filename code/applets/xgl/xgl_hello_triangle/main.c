////////////////////////////////////////////////////////////////////////////////////////////////////

#include "main.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static xgl_pipeline g_pso = {0};
static xgl_shader g_shader = {0};

////////////////////////////////////////////////////////////////////////////////

static result_e _create_pso()
{
    result_e result = RC_FAILURE;

    ////////////////////////////////////////

    xgl_shader shader;
    {
        // vertex shader
        struct xgl_shader_stage_desc vs = {0};
        vs.stage = XGL_SHADER_STAGE_VERTEX;
        vs.src_ptr = g_vs_no_ia_src;

        // fragment shader
        struct xgl_shader_stage_desc fs = {0};
        fs.stage = XGL_SHADER_STAGE_FRAGMENT;
        fs.src_ptr = g_fs_no_ia_src;

        // shader program
        struct xgl_shader_create_info info = {0};
        info.name = make_string("no_ia");
        info.vertex_shader_stage = &vs;
        info.fragment_shader_stage = &fs;

        result = xgl_create_shader(&info, &shader);
        check_result(result, "could not create shader");
    }

    g_shader = shader;

    ////////////////////////////////////////

    struct xgl_ia_state ia_state = {0};
    ia_state.topology = XGL_TOPOLOGY_TRIANGLE_LIST;

    struct xgl_shader_state shader_state = {0};
    shader_state.shader = shader;

    xgl_pipeline pipeline;
    {
        struct xgl_pipeline_create_info info = {0};
        info.name = make_string("no_ia");
        info.type = XGL_PIPELINE_TYPE_GRAPHICS;
        info.ia_state = &ia_state;
        info.shader_state = &shader_state;

        result = xgl_create_pipeline(&info, &pipeline);
        check_result(result, "could not create pipeline");
    }

    g_pso = pipeline;

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct string _plugin_get_name()
{
    return make_string("XGL Hello Triangle");
}

static struct string _plugin_get_description()
{
    return make_string("Rendering a Triangle using XGL");
}

static struct version _plugin_get_version()
{
    return make_version(1, 0, 0);
}

static result_e _plugin_startup()
{
    // add screen
    struct screen_create_info create_info = {0};
    create_info.name = make_string("XGL Triangle");

    create_info.surface.type = SCREEN_SURFACE_TYPE_GPU;
    create_info.surface.clear_values.color = make_vec4(0.22, 0.22, 0.22, 1.0);

    struct screen *screen = aio_add_screen("main", &create_info);
    check_ptr(screen);

    // create pso
    result_e result = _create_pso();
    check_result(result, "could not create pipeline");

    return result;

error:
    return RC_FAILURE;
}

static void _plugin_shutdown()
{
    xgl_destroy_shader(g_shader);
    xgl_destroy_pipeline(g_pso);
}

static void _plugin_tick()
{
    struct screen *screen = aio_get_screen("main");

    if (screen_begin(screen, SCREEN_SURFACE_TYPE_GPU))
    {
        xgl_bind_pipeline(XGL_PIPELINE_TYPE_GRAPHICS, g_pso);
        xgl_draw(0, 3);

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
