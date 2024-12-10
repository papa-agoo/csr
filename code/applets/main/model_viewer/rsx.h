////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rsx/conf.h"
#include "rsx/common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct renderer;

const struct rsx_conf* rsx_get_conf(struct renderer *renderer);

// debug primitives
void rsx_add_point(struct renderer *renderer, struct vec3 p, struct vec3 color, f32 size, f32 lifetime, bool depth);
void rsx_add_line(struct renderer *renderer, struct vec3 a, struct vec3 b, struct vec3 color, f32 width, f32 lifetime, bool depth);

void rsx_add_axes(struct renderer *renderer, struct mat44 transform, bool depth);
void rsx_add_aabb(struct renderer *renderer, struct mat44 transform, struct aabb aabb, bool depth);
