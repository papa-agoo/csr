////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/applet.h>

#include "scene.h"
#include "renderer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mv_conf
{
    struct renderer_conf renderer;
};

struct model_viewer;

result_e model_viewer_init();
void model_viewer_quit();

void model_viewer_tick();

struct model* model_viewer_get_model();
// result_e model_viewer_load_model(struct string path);

struct camera* model_viewer_get_camera();
struct camera_ctl* model_viewer_get_camera_ctl();
void model_viewer_set_camera_ctl_type(enum camera_ctl_type type);

struct mv_conf* model_viewer_get_conf();

#define mv_renderer_conf_ptr() (&model_viewer_get_conf()->renderer)
