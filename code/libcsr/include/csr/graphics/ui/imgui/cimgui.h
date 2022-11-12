////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct cimgui_info
{
    const char* platform_name;
    const char* renderer_name;
};


// ImVec helper
#define make_ImVec2(...) \
    make_compound_literal(struct ImVec2, __VA_ARGS__)

#define make_ImVec3(...) \
    make_compound_literal(struct ImVec3, __VA_ARGS__)

#define make_ImVec4(...) \
    make_compound_literal(struct ImVec4, __VA_ARGS__)

#define make_ImVec2_zero()          make_ImVec2(0, 0)
#define make_ImVec2_one()           make_ImVec2(1, 1)

#define make_ImVec3_zero()          make_ImVec3(0, 0, 0)
#define make_ImVec3_one()           make_ImVec3(1, 1, 1)

#define make_ImVec4_zero()          make_ImVec4(0, 0, 0, 0)
#define make_ImVec4_one()           make_ImVec4(1, 1, 1, 1)

#define make_ImVec2_from_vec2(v)    make_ImVec2(v.x, v.y)
#define make_ImVec3_from_vec3(v)    make_ImVec3(v.x, v.y, v.z)
#define make_ImVec4_from_vec4(v)    make_ImVec4(v.x, v.y, v.z, v.w)
