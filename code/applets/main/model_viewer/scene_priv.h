////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "scene.h"
#include "scene/model_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct scene_cache
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
            struct model plane;
            struct model cube;
            // ...
        } builtin;
    } model;
};

struct scene
{
    struct mesh_node root_node;

    struct model *model;
    struct model_ctl *model_ctl;

    struct camera *camera;
    struct camera_ctl *camera_ctl;

    struct scene_cache cache;
};

result_e scene_init(struct scene *scene);
void scene_quit(struct scene *scene);
