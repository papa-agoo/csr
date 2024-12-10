////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct scene
{
    struct mesh_node root_node;

    struct model *model;
    struct model_ctl *model_ctl;

    struct camera *camera;
    struct camera_ctl *camera_ctl;

    struct scene_conf *conf;
};

struct scene_init_info
{
    struct scene_conf *conf;

    struct camera_create_info *camera_info;
};

result_e scene_init(struct scene_init_info *info, struct scene *scene);
void scene_quit(struct scene *scene);
