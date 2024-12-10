////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rsx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rcpu;

struct rcpu* rcpu_create();
void rcpu_destroy();

void rcpu_tick(struct pixelbuffer *pb, struct softgl_viewport vp);

void rcpu_draw_mesh_primitive(struct mesh_primitive *primitive);

void rcpu_pass_meshes(struct vector *meshes);
void rcpu_pass_gizmos();
void rcpu_pass_debug_primitives();
