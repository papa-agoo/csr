
////////////////////////////////////////////////////////////////////////////////////////////////////

// inputs
layout(location = VERTEX_ATTRIB_LOC_POSITION) in vec3 a_position;
layout(location = VERTEX_ATTRIB_LOC_COLOR) in vec3 a_color;

// outputs
layout(location = 0) out vec3 color;

void main()
{
    color = a_color;

    gl_Position = transform_position(a_position, g_use_object_mvp);
}
