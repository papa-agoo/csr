////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "renderer/mesh.h"
#include "renderer/material.h"

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

    conf->color.background = make_vec3(0.20, 0.20, 0.20);

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

struct renderer;


// meshes
////////////////////////////////////////////////////////////////////////////////////////////////////
// ...


// materials
////////////////////////////////////////////////////////////////////////////////////////////////////
// ...


// debug primitives
////////////////////////////////////////////////////////////////////////////////////////////////////

// screen space
// void renderer_add_point_2d();
// void renderer_add_line_2d();
// void renderer_add_box_2d();
// void renderer_add_text();


// world space
void renderer_add_point(struct renderer *renderer, struct vec3 p, struct vec3 color, f32 size, f32 lifetime, bool depth);
void renderer_add_line(struct renderer *renderer, struct vec3 a, struct vec3 b, struct vec3 color, f32 width, f32 lifetime, bool depth);

// object space
void renderer_add_axes(struct renderer *renderer, struct mat44 transform, bool depth);
void renderer_add_aabb(struct renderer *renderer, struct mat44 transform, struct aabb aabb, bool depth);
