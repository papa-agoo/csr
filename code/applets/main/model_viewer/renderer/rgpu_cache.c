////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet/aio.h> // FIXME

#include "rgpu_priv.h"

#define SHADER_RESOURCE_DIR "{RESOURCE_DIR}/shaders" // FIXME move to application.h

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_depth_stencil_states(struct rgpu_cache *cache);
static result_e _create_rasterizer_states(struct rgpu_cache *cache);
static result_e _create_color_blend_states(struct rgpu_cache *cache);

static result_e _create_input_layouts(struct rgpu_cache *cache);
static result_e _create_descriptor_set_layouts(struct rgpu_cache *cache);
static result_e _create_pipeline_layouts(struct rgpu_cache *cache);

static result_e _create_shaders(struct rgpu_cache *cache);
static result_e _create_pipelines(struct rgpu_cache *cache);
static result_e _create_samplers(struct rgpu_cache *cache);

result_e rgpu_create_cache(struct rgpu_cache *cache)
{
    check_ptr(cache);

    check_result(_create_depth_stencil_states(cache));
    check_result(_create_rasterizer_states(cache));
    check_result(_create_color_blend_states(cache));

    check_result(_create_input_layouts(cache));
    check_result(_create_descriptor_set_layouts(cache));
    check_result(_create_pipeline_layouts(cache));

    check_result(_create_shaders(cache));
    check_result(_create_pipelines(cache));
    check_result(_create_samplers(cache));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void rgpu_destroy_cache(struct rgpu_cache *cache)
{
    check_ptr(cache);

    // descriptor set layouts
    xgl_destroy_descriptor_set_layout(cache->ds_layout.frame);
    xgl_destroy_descriptor_set_layout(cache->ds_layout.pass);
    xgl_destroy_descriptor_set_layout(cache->ds_layout.material);
    xgl_destroy_descriptor_set_layout(cache->ds_layout.object);

    // pipeline layouts
    xgl_destroy_pipeline_layout(cache->pipeline_layout.main);
    // xgl_destroy_pipeline_layout(cache->pipeline_layout.environment);

    // shaders
    xgl_destroy_shader(cache->shader.vertex_color);
    // xgl_destroy_shader(cache->shader.vertex_texture);

    // pipelines
    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        xgl_destroy_pipeline(cache->pipeline.points[i]);
        xgl_destroy_pipeline(cache->pipeline.points_no_depth[i]);

        xgl_destroy_pipeline(cache->pipeline.lines[i]);
        xgl_destroy_pipeline(cache->pipeline.lines_no_depth[i]);
    }

    // xgl_destroy_pipeline(cache->pipeline.debug_uvs);
    // xgl_destroy_pipeline(cache->pipeline.debug_normals);

error:
    return;
}

static result_e _create_depth_stencil_states(struct rgpu_cache *cache)
{
    check_ptr(cache);

    // depth off, stencil off
    {
        static struct xgl_depth_stencil_state state = {0};

        cache->depth_stencil_state.off_off = state;
    }

    // depth r/w, stencil off
    {
        static struct xgl_depth_stencil_state state = {0};
        state.depth.enable_test = true;

        cache->depth_stencil_state.rw_off = state;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_rasterizer_states(struct rgpu_cache *cache)
{
    check_ptr(cache);

    // debug draw
    {
        static struct xgl_rasterizer_state state = {0};

        // FIXME enable smooth points + lines ...

        cache->rasterizer_state.debug_draw = state;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_color_blend_states(struct rgpu_cache *cache)
{
    check_ptr(cache);

    // opaque
    {
        static struct xgl_color_blend_state state = {0};

        cache->color_blend_state.opaque = state;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_input_layouts(struct rgpu_cache *cache)
{
    check_ptr(cache);

    // va_1p
    {
        static struct xgl_vertex_attrib_input va_inputs[1] = {0};

        va_inputs[0].binding = 0;
        va_inputs[0].offset = 0;
        va_inputs[0].location = XGL_VERTEX_ATTRIB_POSITION;

        struct xgl_input_layout input_layout = {0};
        input_layout.attrib_inputs = va_inputs;
        input_layout.attrib_input_count = COUNT_OF(va_inputs);

        cache->input_layout.position = input_layout;
    }

    // va_1p1c
    {
        static struct xgl_vertex_attrib_input va_inputs[2] = {0};

        va_inputs[0].binding = 0;
        va_inputs[0].offset = 0;
        va_inputs[0].location = XGL_VERTEX_ATTRIB_POSITION;

        va_inputs[1].binding = 0;
        va_inputs[1].offset = sizeof(struct vec3);
        va_inputs[1].location = XGL_VERTEX_ATTRIB_COLOR;

        struct xgl_input_layout input_layout = {0};
        input_layout.attrib_inputs = va_inputs;
        input_layout.attrib_input_count = COUNT_OF(va_inputs);

        cache->input_layout.position_color = input_layout;
    }

    // va_1p1uv
    {
        static struct xgl_vertex_attrib_input va_inputs[2] = {0};

        va_inputs[0].binding = 0;
        va_inputs[0].offset = 0;
        va_inputs[0].location = XGL_VERTEX_ATTRIB_POSITION;

        va_inputs[1].binding = 0;
        va_inputs[1].offset = sizeof(struct vec3);
        va_inputs[1].location = XGL_VERTEX_ATTRIB_TEX_COORD0;

        struct xgl_input_layout input_layout = {0};
        input_layout.attrib_inputs = va_inputs;
        input_layout.attrib_input_count = COUNT_OF(va_inputs);

        cache->input_layout.position_texture = input_layout;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_descriptor_set_layouts(struct rgpu_cache *cache)
{
    check_ptr(cache);

    // XGL_DESCRIPTOR_SET_TYPE_FRAME
    {
        struct xgl_descriptor_binding bindings[1];

        bindings[0].type = XGL_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].binding = 0;

        struct xgl_descriptor_set_layout_create_info info = {0};
        info.name = make_string("Frame");
        info.bindings = bindings;
        info.binding_count = COUNT_OF(bindings);

        check_result(xgl_create_descriptor_set_layout(&info, &cache->ds_layout.frame));
    }

    // XGL_DESCRIPTOR_SET_TYPE_PASS
    {
        struct xgl_descriptor_binding bindings[1];

        bindings[0].type = XGL_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].binding = 0;

        struct xgl_descriptor_set_layout_create_info info = {0};
        info.name = make_string("Pass");
        info.bindings = bindings;
        info.binding_count = COUNT_OF(bindings);

        check_result(xgl_create_descriptor_set_layout(&info, &cache->ds_layout.pass));
    }

    // XGL_DESCRIPTOR_SET_TYPE_MATERIAL
    {
        struct xgl_descriptor_binding bindings[1];

        bindings[0].type = XGL_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].binding = 0;

        struct xgl_descriptor_set_layout_create_info info = {0};
        info.name = make_string("Material");
        info.bindings = bindings;
        info.binding_count = COUNT_OF(bindings);

        check_result(xgl_create_descriptor_set_layout(&info, &cache->ds_layout.material));
    }

    // XGL_DESCRIPTOR_SET_TYPE_OBJECT
    {
        struct xgl_descriptor_binding bindings[1];

        bindings[0].type = XGL_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].binding = 0;

        struct xgl_descriptor_set_layout_create_info info = {0};
        info.name = make_string("Object");
        info.bindings = bindings;
        info.binding_count = COUNT_OF(bindings);

        check_result(xgl_create_descriptor_set_layout(&info, &cache->ds_layout.object));
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_pipeline_layouts(struct rgpu_cache *cache)
{
    // main
    {
        xgl_descriptor_set_layout ds_layouts[] = {
            cache->ds_layout.frame,
            cache->ds_layout.pass,
            cache->ds_layout.material,
            cache->ds_layout.object,
        };

        struct xgl_pipeline_layout_create_info info = {0};
        info.ds_layouts = ds_layouts;
        info.ds_layout_count = COUNT_OF(ds_layouts);

        check_result(xgl_create_pipeline_layout(&info, &cache->pipeline_layout.main));
    }

    // environment
    {
        // ...
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void* _load_shader_from_file(string_cstr shader_file)
{
    struct arena *arena = aio_get_frame_arena(); // FIXME

    struct string path = string_create_fmt(arena, "%s/%s", SHADER_RESOURCE_DIR, shader_file);
    path = aio_env_expand_str((string_cstr)path.ptr);

    clog_trace("loading shader source : %s ...", path.ptr);

    struct fio_buffer buffer = {0};
    check_result(fio_load_file(path, &buffer));

    return buffer.data;

error:
    return NULL;
}

static result_e _create_shaders(struct rgpu_cache *cache)
{
    // shader source files
    string_cstr vs_src_common = _load_shader_from_file("common.inc.glsl");
    string_cstr vs_src_vertex_color = _load_shader_from_file("vertex_color.vs.glsl");
    string_cstr fs_src_vertex_color = _load_shader_from_file("vertex_color.fs.glsl");

    // vertex color
    {
        // vertex shader
        string_cstr vs_src_ptrs[] = {
            vs_src_common,
            vs_src_vertex_color
        };

        struct xgl_shader_stage_desc vs_stage = {0};
        vs_stage.stage = XGL_SHADER_STAGE_VERTEX;
        vs_stage.src_ptrs = vs_src_ptrs;
        vs_stage.src_ptr_count = COUNT_OF(vs_src_ptrs);

        // fragment shader
        struct xgl_shader_stage_desc fs_stage = {0};
        fs_stage.stage = XGL_SHADER_STAGE_FRAGMENT;
        fs_stage.src_ptrs = &fs_src_vertex_color;
        fs_stage.src_ptr_count = 1;

        // shader program
        struct xgl_shader_create_info info = {0};
        info.name = make_string("shader.vertex_color");
        info.vertex_shader_stage = &vs_stage;
        info.fragment_shader_stage = &fs_stage;

        check_result(xgl_create_shader(&info, &cache->shader.vertex_color));
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_pso_points(struct rgpu_cache *cache)
{
    check_ptr(cache);

    ////////////////////////////////////////

    struct xgl_ia_state ia_state = {0};
    ia_state.topology = XGL_TOPOLOGY_POINT_LIST;

    struct xgl_shader_state shader_state = {0};
    shader_state.shader = cache->shader.vertex_color;

    struct xgl_pipeline_create_info info = {0};
    info.type = XGL_PIPELINE_TYPE_GRAPHICS;
    info.ia_state = &ia_state;
    info.shader_state = &shader_state;
    info.rasterizer_state = &cache->rasterizer_state.debug_draw;
    info.input_layout = &cache->input_layout.position_color;
    info.pipeline_layout = cache->pipeline_layout.main;

    ////////////////////////////////////////

    struct string pso_names[] = {
        make_string("pso.points"),
        make_string("pso.points.no_depth"),

        make_string("pso.points_thick"),
        make_string("pso.points_thick.no_depth"),

        make_string("pso.points_fat"),
        make_string("pso.points_fat.no_depth"),
    };

    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        info.rasterizer_state->point_size = (i + 1) * (i + 1);

        // with depth
        info.name = pso_names[i];
        info.depth_stencil_state = &cache->depth_stencil_state.rw_off;

        check_result(xgl_create_pipeline(&info, &cache->pipeline.points[i]));

        // without depth
        info.name = pso_names[i + 1];
        info.depth_stencil_state = &cache->depth_stencil_state.off_off;

        check_result(xgl_create_pipeline(&info, &cache->pipeline.points_no_depth[i]));
    }

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_pso_lines(struct rgpu_cache *cache)
{
    check_ptr(cache);

    ////////////////////////////////////////

    struct xgl_ia_state ia_state = {0};
    ia_state.topology = XGL_TOPOLOGY_LINE_LIST;

    struct xgl_shader_state shader_state = {0};
    shader_state.shader = cache->shader.vertex_color;

    struct xgl_pipeline_create_info info = {0};
    info.type = XGL_PIPELINE_TYPE_GRAPHICS;
    info.ia_state = &ia_state;
    info.shader_state = &shader_state;
    info.rasterizer_state = &cache->rasterizer_state.debug_draw;
    info.input_layout = &cache->input_layout.position_color;
    info.pipeline_layout = cache->pipeline_layout.main;

    ////////////////////////////////////////

    struct string pso_names[] = {
        make_string("pso.lines"),
        make_string("pso.lines.no_depth"),

        make_string("pso.lines_thick"),
        make_string("pso.lines_thick.no_depth"),

        make_string("pso.lines_fat"),
        make_string("pso.lines_fat.no_depth"),
    };

    for (u32 i = 0; i < PRIMITIVE_SIZE_MAX; i++)
    {
        info.rasterizer_state->line_width = (i + 1) * (i + 1);

        // with depth
        info.name = pso_names[i];
        info.depth_stencil_state = &cache->depth_stencil_state.rw_off;

        check_result(xgl_create_pipeline(&info, &cache->pipeline.lines[i]));

        // without depth
        info.name = pso_names[i + 1];
        info.depth_stencil_state = &cache->depth_stencil_state.off_off;

        check_result(xgl_create_pipeline(&info, &cache->pipeline.lines_no_depth[i]));
    }

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_pipelines(struct rgpu_cache *cache)
{
    check_ptr(cache);

    check_result(_create_pso_points(cache));
    check_result(_create_pso_lines(cache));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_samplers(struct rgpu_cache *cache)
{
    check_ptr(cache);

    // ...

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
