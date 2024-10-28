////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "renderer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mesh_gizmo
{
    struct string name;

    struct mesh_buffer buffer;
    struct mesh_primitive primitive;
};

struct renderer_shader_data
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
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct renderer
{
    struct renderer_conf conf;

    struct renderer_shader_data shader_data;

    struct {
        struct mesh_gizmo aabb;
        struct mesh_gizmo axes;
        struct mesh_gizmo grid;
    } gizmo;

    struct {
        struct mesh_buffer points;
        struct mesh_buffer lines;
    } debug_draw;

    struct {
        struct screen *rgpu;
        struct screen *rcpu;
    } screen;

    struct rgpu* rgpu;
    struct rcpu* rcpu;
};

result_e renderer_init(struct renderer *renderer);
void renderer_quit(struct renderer *renderer);

void renderer_tick(struct renderer *renderer);

// struct material* renderer_find_suitable_material(struct renderer *renderer, u32 vertex_format);
