////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resources/camera.h"
#include "resources/camera_ctl.h"

#include "resources/model.h"
#include "resources/model_ctl.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mv_resources
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
