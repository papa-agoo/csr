////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct renderer_conf
{
    struct {
        struct vec3 background;

        struct vec3 aabb;
        struct vec3 grid;

        struct vec3 axis_x;
        struct vec3 axis_y;
        struct vec3 axis_z;
    } color;

    bool draw_grid;
    bool draw_aabb;

    bool enable_rgpu;
    bool enable_rcpu;
};

static void renderer_conf_defaults(struct renderer_conf *conf)
{
    check_ptr(conf);

    conf->color.background = make_vec3(0.20f, 0.20f, 0.20f);

    conf->color.aabb = make_vec3(0.75, 0.75, 0.75);
    conf->color.grid = make_vec3(0.35, 0.35, 0.35);

    conf->color.axis_x = make_vec3(0.50, 0.20, 0.20);
    conf->color.axis_y = make_vec3(0.20, 0.48, 0.28);
    conf->color.axis_z = make_vec3(0.20, 0.35, 0.48);

    conf->draw_grid = true;
    conf->draw_aabb = true;

    conf->enable_rgpu = true;
    conf->enable_rcpu = true;

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct renderer
{
    // resources
    struct
    {
        struct {
            struct {
                struct mat44 view;
                struct mat44 projection;
                struct mat44 view_projection;
            } frame;
        } uniform_buffer;

        struct {
            xgl_sampler foo;
        } sampler;

        struct {
            xgl_shader vs_solid;
            xgl_shader vs_wireframe;

            xgl_shader fs_pass_through;
        } shader;

        struct {
            xgl_pipeline debug_uvs;
            xgl_pipeline debug_normals;

            xgl_pipeline points;
            xgl_pipeline wireframe;
            xgl_pipeline solid;
            xgl_pipeline textured;
            xgl_pipeline full;
        } pso;
    } resources;
};
