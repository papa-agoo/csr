////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rcpu_priv.h"
#include "renderer_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_input_layouts(struct rcpu_cache *cache);

static result_e _create_shaders(struct rcpu_cache *cache);
static result_e _create_pipelines(struct rcpu_cache *cache);

result_e rcpu_create_cache(struct rcpu_cache *cache)
{
    check_ptr(cache);

    check_result(_create_input_layouts(cache));

    check_result(_create_shaders(cache));
    check_result(_create_pipelines(cache));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rcpu_destroy_cache(struct rcpu_cache *cache)
{
    check_ptr(cache);

    // shaders
    softgl_destroy_shader(cache->shader.vertex_color);

    // pipelines
    softgl_destroy_pipeline(cache->pipeline.lines);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_input_layouts(struct rcpu_cache *cache)
{
    check_ptr(cache);

    // va_1p
    {
        static struct softgl_vertex_attrib_input va_inputs[1] = {0};

        va_inputs[0].attrib = SOFTGL_VERTEX_ATTRIB_POSITION;
        va_inputs[0].binding = 0;
        va_inputs[0].offset = 0;

        struct softgl_input_layout input_layout = {0};
        input_layout.attrib_inputs = va_inputs;
        input_layout.attrib_input_count = COUNT_OF(va_inputs);

        cache->input_layout.position = input_layout;
    }

    // va_1p1c
    {
        static struct softgl_vertex_attrib_input va_inputs[2] = {0};

        va_inputs[0].attrib = SOFTGL_VERTEX_ATTRIB_POSITION;
        va_inputs[0].binding = 0;
        va_inputs[0].offset = 0;

        va_inputs[1].attrib = SOFTGL_VERTEX_ATTRIB_COLOR;
        va_inputs[1].binding = 0;
        va_inputs[1].offset = sizeof(struct vec3);

        struct softgl_input_layout input_layout = {0};
        input_layout.attrib_inputs = va_inputs;
        input_layout.attrib_input_count = COUNT_OF(va_inputs);

        cache->input_layout.position_color = input_layout;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

extern vertex_shader(vs_vertex_color);
extern fragment_shader(fs_vertex_color);

static result_e _create_shaders(struct rcpu_cache *cache)
{
    // vertex color
    {
        struct softgl_shader_create_info info = {0};
        info.name = make_string("vertex color");
        info.type = SOFTGL_SHADER_TYPE_USER_FUNCTION;
        info.vertex_shader_ptr = vs_vertex_color;
        info.fragment_shader_ptr = fs_vertex_color;

        check_result(softgl_create_shader(&info, &cache->shader.vertex_color));
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_pso_lines(struct rcpu_cache *cache)
{
    struct softgl_ia_state ia_state = {0};
    ia_state.topology = SOFTGL_TOPOLOGY_LINE_LIST;

    struct softgl_shader_state shader_state = {0};
    shader_state.shader = cache->shader.vertex_color;

    struct softgl_pipeline_create_info info = {0};
    info.name = make_string("pso.lines");
    info.ia_state = &ia_state;
    info.shader_state = &shader_state;
    info.input_layout = &cache->input_layout.position_color;

    return softgl_create_pipeline(&info, &cache->pipeline.lines);

error:
    return RC_FAILURE;
}

static result_e _create_pipelines(struct rcpu_cache *cache)
{
    check_result(_create_pso_lines(cache));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
