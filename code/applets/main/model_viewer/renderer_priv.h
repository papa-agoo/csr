////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "renderer.h"
#include "renderer/rgpu.h"
#include "renderer/rcpu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mesh_gizmo
{
    struct string name;

    struct mesh_buffer buffer;
    struct mesh_primitive primitive;
};

struct renderer_cache
{
    struct {
        xgl_sampler linear;
        xgl_sampler nearest;
    } sampler;

    struct {
        xgl_pipeline debug_uvs;
        xgl_pipeline debug_normals;
    } pso;

    struct {
        struct mesh_gizmo aabb;
        struct mesh_gizmo axes;
        struct mesh_gizmo grid;
    } gizmo;

    struct {
        struct material wireframe;
    } material;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct renderer
{
    struct renderer_cache cache;
    struct renderer_conf conf;
};

result_e renderer_init(struct renderer *renderer);
void renderer_quit(struct renderer *renderer);
