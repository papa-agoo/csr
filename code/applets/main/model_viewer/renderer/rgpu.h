////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "renderer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rgpu;

struct rgpu* rgpu_create();
void rgpu_destroy();

void rgpu_tick(struct renderer *renderer, struct xgl_viewport vp);

void rgpu_draw_mesh_primitive(struct mesh_primitive *primitive);

void rgpu_pass_meshes(struct renderer *renderer, struct vector *meshes);
void rgpu_pass_environment(struct renderer *renderer);
void rgpu_pass_gizmos(struct renderer *renderer);
void rgpu_pass_debug_draw(struct renderer *renderer);
