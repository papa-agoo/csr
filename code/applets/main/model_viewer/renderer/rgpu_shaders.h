////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* g_foo =  {
    "#version 450\n"

    "layout (location = 0) in vec3 a_position;\n"
    "layout (location = 0) in vec3 a_position;\n"
};

static const char* g_vs_solid_wireframe = ""
"#version 450\n"
"\n"
"layout (location = 0) in vec3 a_position;\n"
"\n"
"layout (std140, binding = 0) uniform Matrices {\n"
    "mat4 view;\n"
    "mat4 projection;\n"
"} matrices;\n"
"\n"
"layout (std140, binding = 1) uniform Model {\n"
    "mat4 model;\n"
"} model;\n"
"\n"
"out vec3 color;\n"
"\n"
"void main()\n"
"{\n"
    "color = vec3(0.5);\n"
    "gl_Position = matrices.projection * matrices.view * skybox.model * vec4(a_position, 1.0);\n"
"}\n";

static const char* g_fs_color_pass_through = ""
"#version 450\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 color;\n"
"\n"
"void main()\n"
"{\n"
    "FragColor = vec4(color, 1);\n"
"}\n";

static const char* g_vs_debug_uvs = ""
"#version 450\n"
"layout (location = 0) in vec3 a_position;\n"
"layout (location = 2) in vec2 a_uv;\n"
"\n"
"layout (std140, binding = 0) uniform ubo {\n"
    "mat4 model;\n"
    "mat4 view;\n"
    "mat4 projection;\n"
"};\n"
"\n"
"out vec3 color;\n"
"\n"
"void main()\n"
"{\n"
    "color = vec3(a_uv, 0);\n"
    "gl_Position = projection * view * model * vec4(a_position, 1.0);\n"
"}\n";

static const char* g_vs_debug_normals = ""
"#version 450\n"
"layout (location = 0) in vec3 a_position;\n"
"layout (location = 1) in vec3 a_normal;\n"
"\n"
"layout (std140, binding = 0) uniform ubo {\n"
    "mat4 model;\n"
    "mat4 view;\n"
    "mat4 projection;\n"
"};\n"
"\n"
"out vec3 color;\n"
"\n"
"void main()\n"
"{\n"
    "color = mat3(transpose(inverse(model))) * a_normal;\n"
    "gl_Position = projection * view * model * vec4(a_position, 1.0);\n"
"}\n";
