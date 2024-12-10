////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

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

    conf->enable_rgpu = true;
    conf->enable_rcpu = true;

error:
    return;
}
