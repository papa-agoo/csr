////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME >>> include("common.inc.glsl")
#version 450

// vertex attrib locations
#define VERTEX_ATTRIB_LOC_POSITION      0
#define VERTEX_ATTRIB_LOC_COLOR         1
#define VERTEX_ATTRIB_LOC_NORMAL        2
#define VERTEX_ATTRIB_LOC_TEX_COORD0    3
#define VERTEX_ATTRIB_LOC_TEX_COORD1    4
#define VERTEX_ATTRIB_LOC_TANGENT       5
#define VERTEX_ATTRIB_LOC_BITANGENT     6


// descriptor set binding offsets (see enum xgl_descriptor_set_type in xgl/common.h)
#define DESCRIPTOR_SET_FRAME            0
#define DESCRIPTOR_SET_PASS             1
#define DESCRIPTOR_SET_MATERIAL         2
#define DESCRIPTOR_SET_OBJECT           3


// wrapper for "vulkan like" layout support : layout(set = m, binding = n)
#define DESCRIPTOR_SET_UB_COUNT         8
#define DESCRIPTOR_SET_TU_COUNT         4

#define layout_buffer(block_layout, ds_type, n) \
    layout (block_layout, binding = (DESCRIPTOR_SET_UB_COUNT * ds_type) + n)

#define layout_texture(ds_type, n) \
    layout (binding = (DESCRIPTOR_SET_TU_COUNT * ds_type) + n)

// FIXME <<< include("common.inc.glsl")

////////////////////////////////////////////////////////////////////////////////////////////////////

// resource bindings
layout_buffer(std140, DESCRIPTOR_SET_FRAME, 0) uniform frame_data {
    mat4 mat_view;
    mat4 mat_projection;
    mat4 mat_projection_ortho;
};

layout_buffer(std140, DESCRIPTOR_SET_OBJECT, 0) uniform object_data {
    mat4 mat_model;
};

// inputs
layout (location = VERTEX_ATTRIB_LOC_POSITION) in vec3 a_position;
layout (location = VERTEX_ATTRIB_LOC_COLOR) in vec3 a_color;

// outputs
layout (location = 0) out vec3 color;

void main()
{
    color = a_color;

    // gl_Position = mat_projection * mat_view * mat_model * vec4(a_position, 1.0);
    gl_Position = mat_projection * mat_view * vec4(a_position, 1.0);
}
