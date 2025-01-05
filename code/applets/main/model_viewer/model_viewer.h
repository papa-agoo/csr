////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/applet.h>

#include "scene.h"
#include "rsx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct model_viewer_conf
{
    struct scene_conf scene;
    struct rsx_conf rsx;
};

struct model_viewer;

result_e model_viewer_init();
void model_viewer_quit();

void model_viewer_tick();

struct model_viewer_conf* model_viewer_get_conf();

struct scene* model_viewer_get_scene();
void model_viewer_set_camera_controller(enum camera_ctl_type type);

struct model* model_viewer_get_model();
result_e model_viewer_set_model(struct model *model);

struct arena* model_viewer_get_resource_arena(); // FIXME

////////////////////////////////////////////////////////////

#define mv_scene_conf_ptr() (&model_viewer_get_conf()->scene)
#define mv_rsx_conf_ptr() (&model_viewer_get_conf()->rsx)
