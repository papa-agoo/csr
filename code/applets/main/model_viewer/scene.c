////////////////////////////////////////////////////////////////////////////////////////////////////

#include "scene_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e scene_init(struct scene_init_info *info, struct scene *scene)
{
    check_ptr(info);
    check_ptr(info->conf);
    check_ptr(info->camera_info);

    check_ptr(scene);

    ////////////////////////////////////////

    scene->conf = info->conf;

    // init root node
    transform_identity(&scene->root_node.transform);

    // create camera
    scene->camera = camera_create(info->camera_info);
    check(scene->camera, "could not create main camera");

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void scene_quit(struct scene *scene)
{
    check_ptr(scene);
    check_ptr(scene->camera);

    camera_destroy(scene->camera);

error:
    return;
}

const struct scene_conf* scene_get_conf(struct scene *scene)
{
    check_ptr(scene);

    return scene->conf;

error:
    return NULL;
}

struct mesh_node* scene_get_root_node(struct scene *scene)
{
    check_ptr(scene);

    return &scene->root_node;

error:
    return NULL;
}

struct camera* scene_get_camera(struct scene *scene)
{
    check_ptr(scene);

    return scene->camera;

error:
    return NULL;
}

struct camera_ctl* scene_get_camera_ctl(struct scene *scene)
{
    check_ptr(scene);

    return scene->camera_ctl;

error:
    return NULL;
}

struct model* scene_get_model(struct scene *scene)
{
    check_ptr(scene);

    return scene->model;

error:
    return NULL;
}
