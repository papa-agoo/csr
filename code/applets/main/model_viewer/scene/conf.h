////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct scene_conf
{
    struct scene_camera_conf
    {
        struct {
            f32 azimuth;
            f32 polar;
            f32 radius;

            bool interpolate;
            f32 interpolation_speed;
        } orbital;
    } camera;
};

static void scene_conf_defaults(struct scene_conf *conf)
{
    check_ptr(conf);

    struct scene_camera_conf *camera = &conf->camera;
    {
        camera->orbital.azimuth = 75.0;
        camera->orbital.polar = 15.0;
        camera->orbital.radius = 10.0;

        camera->orbital.interpolate = true;
        camera->orbital.interpolation_speed = 6.0f;
    }

error:
    return;
}
