////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/applet.h>

#include "scene.h"
#include "renderer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// NOTES
//
//  - model viewer
//      - very simple user api (load model, set camera, update this, enable that ...)
//      - manages the scene (add, remove, etc.)
//      - manages the renderer (create resources, draw scene setup, etc.)
//
//  - scene
//      - contains entities like model, camera, light, ...
//      - background / environment setup (solid color, gradients, skybox, ...)
//      - fullscreen effects setup (bloom, dof, color correction, ...)
//
//  - renderer
//      - high level render system
//          - resource management for gpu + cpu renderers (geometry, materials, textures, shaders ...)
//          - draw api (debug draw, draw mesh, draw env, draw fsfx, ..)
//      - low level render system
//          - draw api (sort + split + submit)
//          - ...

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mv_conf
{
    struct renderer_conf *renderer;
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

#define mv_renderer_conf_ptr() (model_viewer_get_conf()->renderer)
struct renderer* model_viewer_get_renderer();
