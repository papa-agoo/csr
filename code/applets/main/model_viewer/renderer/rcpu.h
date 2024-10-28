////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "renderer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rcpu;

struct rcpu* rcpu_create();
void rcpu_destroy();

void rcpu_tick(struct renderer *renderer, struct pixelbuffer *pb, struct softgl_viewport vp);

void rcpu_draw_mesh_primitive(struct mesh_primitive *primitive);

void rcpu_pass_meshes(struct renderer *renderer, struct vector *meshes);
void rcpu_pass_gizmos(struct renderer *renderer);
void rcpu_pass_debug_draw(struct renderer *renderer);
