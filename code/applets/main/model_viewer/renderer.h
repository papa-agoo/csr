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

// void renderer_mesh_create();
// void renderer_mesh_destroy();
// ...


// materials
////////////////////////////////////////////////////////////////////////////////////////////////////

// void renderer_material_create();
// void renderer_material_destroy();

// void renderer_effect_create();
// void renderer_effect_destroy();

// void renderer_shader_create();
// void renderer_shader_destroy();

// void renderer_texture_create();
// void renderer_texture_destroy();

// ...


// debug draw
////////////////////////////////////////////////////////////////////////////////////////////////////

// void renderer_debug_draw_point(struct vec3 p, struct vec3 color, f32 size, bool depth_tested);
// void renderer_debug_draw_line(struct vec3 p1, struct vec3 p2, struct vec3 color, f32 width, bool depth_tested);

// void renderer_debug_draw_quad(struct vec3 min, struct vec3 max, const struct mat44 *transform, struct vec3 color, bool depth_tested);

// void renderer_debug_draw_aabb(const struct aabb *aabb, const struct mat44 *transform, bool depth_tested);
// void renderer_debug_draw_axes(const struct mat44 *transform, bool depth_tested);
