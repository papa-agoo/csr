////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "scene/conf.h"

#include "scene/camera.h"
#include "scene/camera_ctl.h"

#include "scene/model.h"
#include "scene/model_ctl.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct scene;

const struct scene_conf* scene_get_conf(struct scene *scene);

struct mesh_node* scene_get_root_node(struct scene *scene);

struct camera* scene_get_camera(struct scene *scene);
struct camera_ctl* scene_get_camera_ctl(struct scene *scene);

struct model* scene_get_model(struct scene *scene);
