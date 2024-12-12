////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../rsx/common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mesh_node
{
    struct rsx_mesh *mesh;

    struct transform transform;

    struct mesh_node *parent;
    // children ...
};

struct model
{
    struct string name;

    struct mesh_node node;

    struct {
        struct arena *arena;
    } resources;
};

struct model_import_info
{
    struct string file_path;

    bool triangulate;
};

struct model_create_info
{
    struct string name;

    struct model_import_info *import;
};

struct model* model_create(struct model_create_info *info);
void model_destroy(struct model* model);
