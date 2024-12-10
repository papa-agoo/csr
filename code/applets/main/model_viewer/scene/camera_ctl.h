////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>
#include <csr/core/math/orbit.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct camera;
struct camera_ctl;

typedef void (*camera_ctl_update_cb)(struct camera *camera, struct camera_ctl *ctl, f64 dt);

enum camera_ctl_type
{
    CAMERA_CTL_NONE = 0,
    CAMERA_CTL_ORBITAL,
    CAMERA_CTL_FIRST_PERSON,
};

struct camera_ctl
{
    enum camera_ctl_type type;

    camera_ctl_update_cb update_cb;

    void *data;
};

struct camera_ctl_orbital
{
    // interpolate from src to dst
    bool interpolate;

    struct orbit orbit_src;
    struct orbit orbit_dst;

    // loop azimuth angle (0 == no animation)
    f32 animate;
};

struct camera_ctl_first_person;
