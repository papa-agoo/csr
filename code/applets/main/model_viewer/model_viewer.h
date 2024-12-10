////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/applet.h>

#include "scene.h"
#include "rsx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct model_viewer_conf
{
    struct scene_conf scene;
    struct rsx_conf renderer;
};

struct model_viewer;

result_e model_viewer_init();
void model_viewer_quit();

void model_viewer_tick();

struct model_viewer_conf* model_viewer_get_conf();

struct scene* model_viewer_get_scene();
void model_viewer_set_camera_controller(enum camera_ctl_type type);

result_e model_viewer_load_model(struct string path);
void model_viewer_unload_model();

struct renderer* model_viewer_get_renderer();

////////////////////////////////////////////////////////////

#define mv_scene_conf_ptr() (&model_viewer_get_conf()->scene)
#define mv_renderer_conf_ptr() (&model_viewer_get_conf()->renderer)
