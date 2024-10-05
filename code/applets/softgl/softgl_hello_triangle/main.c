////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet.h>
#include <csr/graphics/softgl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct my_vertex
{
    struct vec3 position;
    struct vec3 color;
};

struct my_triangle
{
    struct my_vertex vertices[3];

    struct {
        softgl_shader shader;
        softgl_pipeline pso;
    } gpu;
};

struct my_triangle g_triangle = {0};

////////////////////////////////////////////////////////////////////////////////////////////////////

vertex_shader(vs_vertex_color)
{
    vs_in(POSITION, vec3, a_position);
    vs_in(COLOR, vec3, a_color);

    vs_out(vec4, v_color);

    *v_color = make_vec4_3_1(*a_color, 1.0f);

    vs_return( make_vec4_3_1(*a_position, 1.0f) );
}

fragment_shader(fs_vertex_color)
{
    fs_in(0, vec4, v_color);

    fs_return( *v_color );
}

static result_e _create_pso()
{
    // ia state
    struct softgl_ia_state ia_state = {0};
    ia_state.topology = SOFTGL_TOPOLOGY_TRIANGLE_LIST;

    // input layout
    struct softgl_vertex_attrib_input va_inputs[2] = {0};

    va_inputs[0].attrib = SOFTGL_VERTEX_ATTRIB_POSITION;
    va_inputs[0].binding = 0;
    va_inputs[0].offset = 0;

    va_inputs[1].attrib = SOFTGL_VERTEX_ATTRIB_COLOR;
    va_inputs[1].binding = 0;
    va_inputs[1].offset = sizeof(struct vec3);

    struct softgl_input_layout input_layout = {0};
    input_layout.attrib_inputs = va_inputs;
    input_layout.attrib_input_count = COUNT_OF(va_inputs);

    // shader state
    softgl_shader shader;
    {
        struct softgl_shader_create_info info = {0};
        info.name = make_string("vertex color");
        info.type = SOFTGL_SHADER_TYPE_USER_FUNCTION;
        info.vertex_shader_ptr = vs_vertex_color;
        info.fragment_shader_ptr = fs_vertex_color;

        check_result(softgl_create_shader(&info, &shader));

        g_triangle.gpu.shader = shader;
    }

    struct softgl_shader_state shader_state = {0};
    shader_state.shader = shader;

    // pipeline
    softgl_pipeline pipeline;
    {
        struct softgl_pipeline_create_info info = {0};
        info.name = make_string("hello_triangle_pipeline");
        info.ia_state = &ia_state;
        info.shader_state = &shader_state;
        info.input_layout = &input_layout;

        check_result(softgl_create_pipeline(&info, &pipeline));

        g_triangle.gpu.pso = pipeline;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct string _plugin_get_name()
{
    return make_string("SoftGL Hello Triangle");
}

static struct string _plugin_get_description()
{
    return make_string("Rendering a Triangle using SoftGL");
}

static struct version _plugin_get_version()
{
    return make_version(1, 0, 0);
}

static result_e _plugin_startup()
{
    // add screen
    struct screen_create_info create_info = {0};
    create_info.name = "SoftGL Triangle";

    create_info.surface.type = SCREEN_SURFACE_TYPE_CPU;
    create_info.surface.clear_values.color = make_vec4(0.22, 0.22, 0.22, 1.0);
    create_info.surface.viewport.width = 640;
    create_info.surface.viewport.height = 360;

    struct screen *screen = aio_add_screen("main", &create_info);
    check_ptr(screen);

    // init softgl
    check_expr(softgl_init_driver() == RC_SUCCESS);

    // create triangle
    struct my_triangle *t = &g_triangle;
    t->vertices[0] = (struct my_vertex){{ 0.5f,-0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}};
    t->vertices[1] = (struct my_vertex){{ 0.0f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}};
    t->vertices[2] = (struct my_vertex){{-0.5f,-0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}};

    // create pipeline
    check_expr(_create_pso() == RC_SUCCESS);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _plugin_shutdown()
{
    softgl_destroy_shader(g_triangle.gpu.shader);
    softgl_destroy_pipeline(g_triangle.gpu.pso);

    softgl_quit_driver();
}

static void _plugin_tick()
{
    struct screen *screen = aio_get_screen("main");

    if (screen_begin(screen, SCREEN_SURFACE_TYPE_CPU))
    {
        struct pixelbuffer *pb = screen_get_pixelbuffer(screen);

        struct softgl_viewport vp = {0};
        vp.width = pb->width;
        vp.height = pb->height;
        vp.max_depth = 1;

        softgl_set_viewport(vp);
        softgl_set_pixelbuffer(pb);

        struct softgl_vertex_buffer vb = {0};
        vb.buffer.data = g_triangle.vertices;
        vb.buffer.byte_length = sizeof(g_triangle.vertices);
        vb.stride = sizeof(struct my_vertex);

        struct softgl_vertex_buffer* vertex_buffers[] = {
            &vb
        };

        softgl_bind_vertex_buffers(vertex_buffers, 1);

        softgl_bind_pipeline(g_triangle.gpu.pso);
        softgl_draw(0, 3);

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
