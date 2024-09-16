////////////////////////////////////////////////////////////////////////////////////////////////////

#include "main.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct my_cube g_cube = {0};

// samplers
u32 g_sampler_idx = 2;
xgl_sampler g_samplers[3];

// textures
#define TEXTURE_IMAGE_DIR "{RESOURCE_DIR}/images/debug"

u32 g_texture_idx = 0;
struct texture_info g_textures[] = {
    {"uv_grid_blender_256.png", 0, 0},
    {"uv_grid_opengl_1024.jpg", 0, 0},
};
const size_t g_textures_count = COUNT_OF(g_textures);

////////////////////////////////////////////////////////////////////////////////////////////////////

static f32 g_cube_positions[] = {
    -1.0, -1.0,  1.0, 1.0, -1.0,  1.0, 1.0,  1.0,  1.0, -1.0,  1.0,  1.0,   // front
    -1.0,  1.0,  1.0, 1.0,  1.0,  1.0, 1.0,  1.0, -1.0, -1.0,  1.0, -1.0,   // top
     1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0, -1.0, 1.0,  1.0, -1.0,  // back
    -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0,  1.0, -1.0, -1.0,  1.0,   // bottom
    -1.0, -1.0, -1.0, -1.0, -1.0,  1.0, -1.0,  1.0,  1.0, -1.0,  1.0, -1.0, // left
     1.0, -1.0,  1.0, 1.0, -1.0, -1.0, 1.0,  1.0, -1.0, 1.0,  1.0,  1.0,    // right
  };

static f32 g_cube_tex_coords[] = {
    0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, // front
    0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, // right
    0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, // back
    0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, // left
    0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, // bottom
    0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, // top
};

static u32 g_cube_elements[] = {
   0,  1,  2, 2,  3,  0,    // front
   4,  5,  6, 6,  7,  4,    // top
   8,  9, 10, 10, 11,  8,   // back
  12, 13, 14, 14, 15, 12,   // bottom
  16, 17, 18, 18, 19, 16,   // left
  20, 21, 22, 22, 23, 20,   // right
};

////////////////////////////////////////////////////////////////////////////////////////////////////

// layout location 0 : XGL_VERTEX_ATTRIB_POSITION
// layout location 3 : XGL_VERTEX_ATTRIB_TEX_COORD0

static u8* g_cube_vs = ""
"#version 450\n"
"\n"
"layout (location = 0) in vec3 a_position;\n"
"layout (location = 3) in vec2 a_tex_coord0;\n"
"\n"
"layout (std140, binding = 0) uniform shader_params {\n"
    "mat4 mvp;\n"
    "float texture_scale;\n"
"};\n"
"\n"
"out vec2 ourTexCoord;\n"
"\n"
"void main()\n"
"{\n"
    "gl_Position = mvp * vec4(a_position, 1.0);\n"
    "ourTexCoord = a_tex_coord0 * texture_scale;\n"
"}\n";

static u8* g_cube_fs = ""
"#version 450\n"
"\n"
"uniform sampler2D ourTexture;\n"
"\n"
"in vec2 ourTexCoord;\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
    "FragColor = texture(ourTexture, ourTexCoord);\n"
"}\n";

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_pso()
{
    result_e result = RC_FAILURE;

    ////////////////////////////////////////

    // ia state
    struct xgl_ia_state ia_state = {0};
    ia_state.topology = XGL_TOPOLOGY_TRIANGLE_LIST;

    ////////////////////////////////////////

    xgl_shader shader;
    {
        // vertex shader
        struct xgl_shader_stage_desc vs_stage = {0};
        vs_stage.stage = XGL_SHADER_STAGE_VERTEX;
        vs_stage.src_ptr = g_cube_vs;

        // fragment shader
        struct xgl_shader_stage_desc stage_fs = {0};
        stage_fs.stage = XGL_SHADER_STAGE_FRAGMENT;
        stage_fs.src_ptr = g_cube_fs;

        // shader program
        struct xgl_shader_create_info info = {0};
        info.name = "xgl_hello_cube";
        info.vertex_shader_stage = &vs_stage;
        info.fragment_shader_stage = &stage_fs;

        result = xgl_create_shader(&info, &shader);
        check_result(result, "could not create shader");

        g_cube.material.effect.shader = shader;
    }

    // shader state
    struct xgl_shader_state shader_state = {0};
    shader_state.shader = shader;

    ////////////////////////////////////////

    // input layout
    struct xgl_vertex_attrib_input va_inputs[2] = {0};

    va_inputs[0].binding = 0;
    va_inputs[0].location = XGL_VERTEX_ATTRIB_POSITION;

    va_inputs[1].binding = 1;
    va_inputs[1].location = XGL_VERTEX_ATTRIB_TEX_COORD0;

    struct xgl_input_layout input_layout = {0};
    input_layout.attrib_inputs = va_inputs;
    input_layout.attrib_input_count = COUNT_OF(va_inputs);

    ////////////////////////////////////////

    // descriptor set layout
    xgl_descriptor_set_layout ds_layout;
    {
        struct xgl_descriptor_binding bindings[2];

        bindings[0].type = XGL_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].binding = 0;

        bindings[1].type = XGL_DESCRIPTOR_TYPE_TEXTURE_SAMPLER;
        bindings[1].binding = 0;

        struct xgl_descriptor_set_layout_create_info info = {0};
        info.bindings = bindings;
        info.binding_count = COUNT_OF(bindings);

        result_e result = xgl_create_descriptor_set_layout(&info, &ds_layout);
        check_result(result, "could not create ds layout");

        g_cube.descriptor_set_layout = ds_layout;
    }

    // pipeline layout
    xgl_pipeline_layout pipeline_layout;
    {
        xgl_descriptor_set_layout ds_layouts[] = {
            ds_layout,
        };

        struct xgl_pipeline_layout_create_info info = {0};
        info.ds_layouts = ds_layouts;
        info.ds_layout_count = COUNT_OF(ds_layouts);

        result_e result = xgl_create_pipeline_layout(&info, &pipeline_layout);
        check_result(result, "could not create pipeline layout");

        g_cube.material.effect.pipeline_layout = pipeline_layout;
    }

    ////////////////////////////////////////

    struct xgl_depth_stencil_state ds = {0};
    ds.depth.enable_test = true;
    ds.depth.compare_op = XGL_COMPARE_OP_GREATER; // FIXME wtf

    ////////////////////////////////////////

    struct xgl_rasterizer_state rs = {0};
    rs.cull_mode = XGL_CULL_MODE_BACK;

    ////////////////////////////////////////

    // create pipeline
    xgl_pipeline pipeline;
    {
        struct xgl_pipeline_create_info info = {0};

        info.name = "hello_cube_pipeline";
        info.type = XGL_PIPELINE_TYPE_GRAPHICS;
        info.ia_state = &ia_state;
        info.shader_state = &shader_state;
        info.depth_stencil_state = &ds;
        info.rasterizer_state = &rs;
        info.input_layout = &input_layout;
        info.pipeline_layout = pipeline_layout;

        result = xgl_create_pipeline(&info, &pipeline);
        check_result(result, "could not create pipeline: %s", info.name);

        g_cube.material.effect.pipeline = pipeline;
    }

error:
    return result;
}

static result_e _create_buffers()
{
    result_e result = RC_FAILURE;

    ////////////////////////////////////////

    // vertex positions
    xgl_buffer vbo_positions;
    {
        struct xgl_buffer_create_info info = {};
        info.usage_flags = XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        info.byte_length = sizeof(f32) * COUNT_OF(g_cube_positions);
        info.data = g_cube_positions;

        result = xgl_create_buffer(&info, &vbo_positions);
        check_result(result, "could not create vertex buffer: %s", "positions");

        g_cube.geometry.vertex_buffer.positions = vbo_positions;
    }

    ////////////////////////////////////////

    // vertex tex-coords
    xgl_buffer vbo_tex_coords;
    {
        struct xgl_buffer_create_info info = {};
        info.usage_flags = XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        info.byte_length = sizeof(f32) * COUNT_OF(g_cube_tex_coords);
        info.data = g_cube_tex_coords;

        result = xgl_create_buffer(&info, &vbo_tex_coords);
        check_result(result, "could not create vertex buffer: %s", "tex_coords");

        g_cube.geometry.vertex_buffer.tex_coords = vbo_tex_coords;
    }

    ////////////////////////////////////////

    // vertex indices
    xgl_buffer index_buffer;
    {
        u32 index_count = COUNT_OF(g_cube_elements);

        struct xgl_buffer_create_info info = {};
        info.usage_flags = XGL_BUFFER_USAGE_INDEX_BUFFER_BIT;
        info.byte_length = index_count * sizeof(u32);
        info.data = g_cube_elements;

        result = xgl_create_buffer(&info, &index_buffer);
        check_result(result, "could not create index buffer");

        g_cube.geometry.index_buffer.indices = index_buffer;
        g_cube.geometry.index_buffer.index_count = index_count;
    }

    ////////////////////////////////////////

    // uniform buffer
    xgl_buffer uniform_buffer;
    {
        struct xgl_buffer_create_info info = {};
        info.usage_flags = XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        info.byte_length = sizeof(struct cube_data);
        info.data = &g_cube.material.shader_params.cpu;

        result = xgl_create_buffer(&info, &uniform_buffer);
        check_result(result, "could not create uniform buffer");

        g_cube.material.shader_params.gpu = uniform_buffer;
    }

    ////////////////////////////////////////

error:
    return result;
}

static result_e _create_texture(struct string image_dir, u32 texture_id, u32 mip_levels)
{
    struct texture_info* tex_info = &g_textures[texture_id];

    ////////////////////////////////////////

    struct string filename = string_create_fmt(aio_get_main_arena(), string_fmt"/%s",
        string_fmt_arg(image_dir), tex_info->filename);

    alog_trace("loading image file ("string_fmt") ...", string_fmt_arg(filename));

    tex_info->image = pixelbuffer_create_from_file(filename, true);
    check_ptr(tex_info->image);

    ////////////////////////////////////////

    result_e result = RC_FAILURE;

    xgl_texture texture;
    {
        struct xgl_texture_create_info info = {};
        info.type = XGL_TEXTURE_TYPE_2D;
        info.format = XGL_TEXTURE_FORMAT_RGBA;
        info.usage_flags = XGL_TEXTURE_USAGE_SAMPLED_BIT;
        info.width = tex_info->image->width;
        info.height = tex_info->image->height;
        info.mip_level_count = mip_levels;
        info.array_layer_count = 1;

        // allocate storage
        result = xgl_create_texture(&info, &texture);
        check_result(result, "could not create texture : %d", texture_id);

        // copy image data
        result = xgl_update_texture(texture, 0, info.width, info.height, (u8*)tex_info->image->pixels);

        if (!R_SUCCESS(result)){
            clog_warn("could not update texture : %d", texture_id);
        }

        tex_info->texture = texture;
    }

    return result;

error:

    return RC_FAILURE;
}

static result_e _create_samplers()
{
    result_e result = RC_FAILURE;

    // nearest filtering
    xgl_sampler* sampler_nearest = g_samplers;
    {
        struct xgl_sampler_desc ci = {0};
        ci.name = "nearest";
        ci.min_filter = XGL_TEXTURE_FILTER_NEAREST;
        ci.mag_filter = XGL_TEXTURE_FILTER_NEAREST;

        result = xgl_create_sampler(&ci, sampler_nearest);
        check_result(result, "could not create sampler : %s", ci.name);
    }

    // linear filtering
    xgl_sampler* sampler_linear = g_samplers + 1;
    {
        struct xgl_sampler_desc ci = {0};
        ci.name = "linear";

        result = xgl_create_sampler(&ci, sampler_linear);
        check_result(result, "could not create sampler : %s", ci.name);
    }

    // anisotropic filtering
    xgl_sampler* sampler_linear_af16x = g_samplers + 2;
    {
        struct xgl_sampler_desc ci = {0};
        ci.name = "linear af16x";
        ci.max_anisotropy = 16.0f;

        result = xgl_create_sampler(&ci, sampler_linear_af16x);
        check_result(result, "could not create sampler : %s", ci.name);
    }

error:
    return result;
}

static result_e _create_shader_bindings()
{
    result_e result = RC_FAILURE;

    result = xgl_create_descriptor_set(g_cube.descriptor_set_layout, &g_cube.descriptor_set);
    check_result(result, "could not create descriptor set");

    struct xgl_buffer_descriptor shader_params = {0};
    shader_params.binding = 0;
    shader_params.buffer = g_cube.material.shader_params.gpu;

    struct xgl_texture_descriptor cube_texture = {0};
    cube_texture.binding = 0;
    cube_texture.sampler = g_cube.material.sampler;
    cube_texture.texture = g_cube.material.texture;

    struct xgl_descriptor_set_update_info info = {0};
    info.buffer_descriptors = &shader_params;
    info.buffer_descriptor_count = 1;
    info.texture_descriptors = &cube_texture;
    info.texture_descriptor_count = 1;

    result = xgl_update_descriptor_set(g_cube.descriptor_set, &info);
    check_result(result, "could not update descriptor set");

error:
    return result;
}


result_e create_resources()
{
    result_e result = _create_pso();
    check_result(result, "could not create pipeline");

    result = _create_buffers();
    check_result(result, "could not create buffers");

    result = _create_samplers();
    check_result(result, "could not create samplers");

    ////////////////////////////////////////

    struct string image_dir = aio_env_expand_str(TEXTURE_IMAGE_DIR);

    for (u32 i = 0; i < COUNT_OF(g_textures); i++)
    {
        u32 mip_levels = 8;

        result = _create_texture(image_dir, i, mip_levels);
        check_result(result, "could not create textures");
    }

    ////////////////////////////////////////

    // init shader params
    g_cube.material.shader_params.cpu.mvp = mat44_identity();
    g_cube.material.shader_params.cpu.texture_scale = 1.0f;

    // init shader bindings
    g_cube.material.sampler = g_samplers[g_sampler_idx];
    g_cube.material.texture = g_textures[g_texture_idx].texture;

    result = _create_shader_bindings();
    check_result(result, "could not create descriptor set");

error:
    return result;
}

void destroy_resources()
{
    for (u32 i = 0; i < COUNT_OF(g_samplers); i++) {
        xgl_destroy_sampler(g_samplers[i]);
    }

    for (u32 i = 0; i < COUNT_OF(g_textures); i++)
    {
        struct texture_info* tex_info = &g_textures[i];

        pixelbuffer_destroy(tex_info->image);
        xgl_destroy_texture(tex_info->texture);
    }

    xgl_destroy_buffer(g_cube.geometry.vertex_buffer.tex_coords);
    xgl_destroy_buffer(g_cube.geometry.vertex_buffer.positions);
    xgl_destroy_buffer(g_cube.geometry.index_buffer.indices);
    xgl_destroy_buffer(g_cube.material.shader_params.gpu);

    xgl_destroy_descriptor_set(g_cube.descriptor_set);
    xgl_destroy_descriptor_set_layout(g_cube.descriptor_set_layout);

    xgl_destroy_shader(g_cube.material.effect.shader);
    xgl_destroy_pipeline(g_cube.material.effect.pipeline);
    xgl_destroy_pipeline_layout(g_cube.material.effect.pipeline_layout);
}
