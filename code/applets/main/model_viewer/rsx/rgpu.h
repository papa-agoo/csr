////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rsx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rgpu;

struct rgpu* rgpu_create();
void rgpu_destroy();

void rgpu_tick(struct xgl_viewport vp);

void rgpu_draw_mesh_primitive(struct mesh_primitive *primitive);

void rgpu_pass_meshes(struct vector *meshes);
void rgpu_pass_environment();
void rgpu_pass_gizmos();
void rgpu_pass_debug_primitives();
