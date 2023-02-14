////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/applet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static char *g_vs_no_ia_src = ""
"#version 330 core\n"
"\n"
"out vec4 ourColor;\n"
"\n"
"const vec2 positions[3] = vec2[3](\n"
"   vec2(-0.5f, -0.5f),\n"
"   vec2( 0.0f,  0.5f),\n"
"   vec2( 0.5f, -0.5f)\n"
");\n"
"\n"
"const vec3 colors[3] = vec3[3](\n"
"   vec3(1, 0, 0),\n"
"   vec3(0, 1, 0),\n"
"   vec3(0, 0, 1)\n"
");\n"
"\n"
"void main()\n"
"{\n"
    "gl_Position = vec4(positions[gl_VertexID], 0, 1);\n"
    "ourColor = vec4(colors[gl_VertexID], 1);\n"
"}\n";

static char* g_fs_no_ia_src = ""
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec4 ourColor;\n"
"\n"
"void main()\n"
"{\n"
    "FragColor = ourColor;\n"
"}\n";
