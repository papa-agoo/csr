////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rsx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME >>> use mesh_node
struct mesh_gizmo
{
    struct string name;

    struct mesh_buffer buffer;
    struct mesh_primitive primitive;

    struct shader_data_object data;
};
// FIXME <<< use mesh_node

struct rsx_shader_data
{
    // frame
    struct {
        struct {
            struct shader_data_frame cpu;
            xgl_buffer gpu;
        } buffer;

        xgl_descriptor_set ds;
    } frame;

    // pass main
    struct {
        struct {
            struct shader_data_pass cpu;
            xgl_buffer gpu;
        } buffer;

        xgl_descriptor_set ds;
    } pass_main;

    // pass environment
    struct {
        xgl_descriptor_set ds;
    } pass_environment;

    // object (fallback)
    struct {
        struct {
            struct shader_data_object cpu;
            xgl_buffer gpu;
        } buffer;

        xgl_descriptor_set ds;
    } object;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rsx
{
    struct rsx_conf *conf;

    struct rsx_shader_data shader_data;

    struct {
        struct mesh_gizmo axes;
        struct mesh_gizmo grid;
    } gizmo;

    struct {
        struct mesh_buffer points[PRIMITIVE_SIZE_MAX];
        struct mesh_buffer points_no_depth[PRIMITIVE_SIZE_MAX];

        struct mesh_buffer lines[PRIMITIVE_SIZE_MAX];
        struct mesh_buffer lines_no_depth[PRIMITIVE_SIZE_MAX];
    } debug_draw;

    struct {
        struct screen *rgpu;
        struct screen *rcpu;
    } screen;

    struct rgpu* rgpu;
    struct rcpu* rcpu;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rsx_init_info
{
    struct rsx_conf *conf;

    struct screen *screen_rgpu;
    struct screen *screen_rcpu;
};

struct rsx* rsx_ptr();

result_e rsx_init(struct rsx_init_info *info);
void rsx_quit();

void rsx_tick();

void rsx_calc_axes_viewport(f32 *x, f32 *y, f32 *width, f32 *height);
// struct material* renderer_find_suitable_material(struct renderer *renderer, u32 vertex_format);
