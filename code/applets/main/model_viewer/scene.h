////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "scene/camera.h"
#include "scene/camera_ctl.h"

#include "scene/model.h"
#include "scene/model_ctl.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct scene
{
    // cameras
    struct {
        struct camera *main;

        struct {
            struct camera_ctl none;
            struct camera_ctl orbital;
            struct camera_ctl first_person;
        } controller;
    } camera;

    // models
    struct {
        struct model *dummy;

        struct {
            struct model *axes;
            struct model *grid;
        } gizmo;
    } model;
};
