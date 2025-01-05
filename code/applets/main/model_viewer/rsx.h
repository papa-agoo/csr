////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rsx/conf.h"
#include "rsx/common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rsx_init_info
{
    struct rsx_conf *conf;

    struct screen *screen_rgpu;
    struct screen *screen_rcpu;

    struct arena *arena;
};

result_e rsx_init(struct rsx_init_info *info);
void rsx_quit();

void rsx_tick(f64 dt);

const struct rsx_conf* rsx_get_conf();
struct rsx_render_data* rsx_get_render_data();


////////////////////////////////////////////////////////////////////////////////
// resource api
////////////////////////////////////////////////////////////////////////////////

// textures
// ...


// render targets
// ...


// shaders
// ...


// materials
struct rsx_material* rsx_material_create(struct rsx_material_create_info *info);
struct rsx_material* rsx_material_copy(struct rsx_material *material);
void rsx_material_destroy(struct rsx_material *material);

struct rsx_material* rsx_material_find_suitable(u32 vertex_format);


// meshes
struct rsx_mesh* rsx_mesh_create(struct rsx_mesh_create_info *info);
struct rsx_mesh* rsx_mesh_copy(struct rsx_mesh *mesh);
void rsx_mesh_destroy(struct rsx_mesh *mesh);

struct rsx_mesh* rsx_mesh_create_test_triangle();
struct rsx_mesh* rsx_mesh_create_test_box();

////////////////////////////////////////////////////////////////////////////////
// command api
////////////////////////////////////////////////////////////////////////////////

// debug primitives
void rsx_debug_add_point(struct vec3 p, struct vec3 color, f32 size, f32 lifetime, bool depth);
void rsx_debug_add_line(struct vec3 a, struct vec3 b, struct vec3 color, f32 width, f32 lifetime, bool depth);

void rsx_debug_add_axes(struct mat44 transform, bool depth);
void rsx_debug_add_aabb(struct mat44 transform, struct aabb aabb, bool depth);
void rsx_debug_add_colored_aabb(struct mat44 transform, struct aabb aabb, struct vec3 color, bool depth);

// ...
