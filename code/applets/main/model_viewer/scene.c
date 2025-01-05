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

void scene_tick(struct scene *scene, f64 dt)
{
    check_ptr(scene);
    check_ptr(scene->camera);

    struct camera *camera = scene->camera;
    struct camera_ctl *camera_ctl = scene->camera_ctl;

    // update camera
    if (camera_ctl->update_cb) {
        camera_ctl->update_cb(camera, camera_ctl, dt);
    }

    // update model
    // ...

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

struct vec3 scene_get_origin(struct scene *scene)
{
    check_ptr(scene);

    struct camera_ctl *ctl = scene->camera_ctl;

    if (ctl && ctl->type == CAMERA_CTL_ORBITAL)
    {
        struct camera_ctl_orbital *data = ctl->data;
        struct orbit *orbit = &data->orbit_src;

        return orbit->origin;
    }

error:
    return make_vec3_zero();
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

void scene_set_model(struct scene *scene, struct model *model)
{
    check_ptr(scene);

    scene->model = model;

    struct mesh_node *parent = &scene->root_node;
    transform_identity(&parent->transform);

    if (model) {
        model->node.parent = parent;
    }

error:
    return;
}
