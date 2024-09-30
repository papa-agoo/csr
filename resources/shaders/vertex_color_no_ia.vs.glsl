////////////////////////////////////////////////////////////////////////////////////////////////////
// vs : vertex color
////////////////////////////////////////////////////////////////////////////////////////////////////
#version 450

layout (std140, binding = 0) uniform frame_data {
    mat4 mat_view;
    mat4 mat_projection;
    mat4 mat_projection_ortho;
};

out vec3 color;

void main()
{
    const vec2 positions[3] = vec2[3](
        vec2(-0.5f, -0.5f),
        vec2( 0.0f,  0.5f),
        vec2( 0.5f, -0.5f)
    );

    const vec3 colors[3] = vec3[3](
        vec3(1, 0, 0),
        vec3(0, 1, 0),
        vec3(0, 0, 1)
    );

    gl_Position = vec4(positions[gl_VertexID].xy, 0, 1);
    color = colors[gl_VertexID];
}
