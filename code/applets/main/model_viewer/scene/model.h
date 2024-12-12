////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rsx/common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mesh_node
{
    struct aabb aabb;

    struct rsx_mesh *mesh;

    struct transform transform;

    struct mesh_node *parent;
    // children ...
};

struct model;

struct model_create_info
{
    struct string name;
    struct string file_path;
};

struct model* model_create(struct model_create_info *create_info);
void model_destroy(struct model* model);

struct string model_get_name(struct model *model);
struct aabb model_get_aabb(struct model* model);

struct transform* model_get_transform(struct model *model);
void model_set_transform(struct model *model, struct transform *transform);

struct transform* model_get_parent_transform(struct model *model);
void model_set_parent_transform(struct model *model, struct transform *transform);

struct mat44 model_get_matrix(struct model *model);
struct mat44 model_get_world_matrix(struct model *model);
