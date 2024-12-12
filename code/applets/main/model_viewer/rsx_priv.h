////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rsx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rsx_resources
{
    struct object_pool *meshes;
    struct object_pool *materials;
    struct object_pool *textures;

    // ...
};

struct rsx
{
    // FIXME rsx_render_target
    struct {
        struct screen *rgpu;
        struct screen *rcpu;
    } screen;

    // resources
    struct rsx_resources resources;

    // high level render data
    struct rsx_render_data render_data;

    // low level renderers
    struct rgpu* rgpu;
    struct rcpu* rcpu;

    // global config
    struct rsx_conf *conf;
};

////////////////////////////////////////////////////////////

struct rsx* rsx_ptr();

result_e rsx_pass_meshes_create(struct rsx_pass_meshes *pass_data);
void rsx_pass_meshes_destroy(struct rsx_pass_meshes *pass_data);

result_e rsx_pass_gizmos_create(struct rsx_pass_gizmos *pass_data);
void rsx_pass_gizmos_destroy(struct rsx_pass_gizmos *pass_data);

result_e rsx_pass_environment_create(struct rsx_pass_environment *pass_data);
void rsx_pass_environment_destroy(struct rsx_pass_environment *pass_data);

result_e rsx_pass_debug_primitives_create(struct rsx_pass_debug_primitives *pass_data);
void rsx_pass_debug_primitives_destroy(struct rsx_pass_debug_primitives *pass_data);

////////////////////////////////////////////////////////////

#define rsx_resource_ptr() (&rsx_ptr()->resources)

#define rsx_rgpu_ptr() (rsx_ptr()->rgpu)
#define rsx_rgpu_cache_ptr() (&rsx_rgpu_ptr()->cache)

#define rsx_rcpu_ptr() (rsx_ptr()->rcpu)
#define rsx_rcpu_cache_ptr() (&rsx_rcpu_ptr()->cache)

#define rsx_pass_data_ptr() (&rsx_get_render_data()->pass)
#define rsx_pass_data_meshes_ptr() (&rsx_pass_data_ptr()->meshes)
#define rsx_pass_data_gizmos_ptr() (&rsx_pass_data_ptr()->gizmos)
#define rsx_pass_data_environment_ptr() (&rsx_pass_data_ptr()->environment)
#define rsx_pass_data_debug_primitives_ptr() (&rsx_pass_data_ptr()->debug_primitives)
