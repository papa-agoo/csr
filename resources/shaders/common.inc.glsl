////////////////////////////////////////////////////////////////////////////////////////////////////
// common definitions
////////////////////////////////////////////////////////////////////////////////////////////////////
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
