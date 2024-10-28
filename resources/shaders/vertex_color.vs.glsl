
////////////////////////////////////////////////////////////////////////////////////////////////////

// resource bindings
layout_buffer(std140, DESCRIPTOR_SET_FRAME, 0)uniform frame_data {
    mat4 mat_view;
    mat4 mat_projection;
    mat4 mat_projection_ortho;
};

layout_buffer(std140, DESCRIPTOR_SET_OBJECT, 0)uniform object_data {
    mat4 mat_model;
    mat4 mat_mvp;
};

// inputs
layout(location = VERTEX_ATTRIB_LOC_POSITION) in vec3 a_position;
layout(location = VERTEX_ATTRIB_LOC_COLOR) in vec3 a_color;

// outputs
layout(location = 0) out vec3 color;

void main()
{
    color = a_color;

    // gl_Position = mat_projection * mat_view * mat_model * vec4(a_position, 1.0);
    gl_Position = mat_projection * mat_view * vec4(a_position, 1.0);
}
