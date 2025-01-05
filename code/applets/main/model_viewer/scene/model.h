////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../rsx.h"

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
    } priv;
};

struct model_import_info
{
    struct string file_path;

    bool triangulate;
};

enum model_mesh_type
{
    MODEL_MESH_TYPE_NONE = 0,

    MODEL_MESH_TYPE_TEST_TRIANGLE,
    MODEL_MESH_TYPE_TEST_BOX,

    MODEL_MESH_TYPE_IMPORT_FROM_FILE,
};

struct model_create_info
{
    struct string name;
    enum model_mesh_type mesh_type;

    struct model_import_info import;

    struct arena *arena; // FIXME resource_srv
};

struct model* model_create(struct model_create_info *info);
void model_destroy(struct model* model);

struct rsx_mesh* model_get_mesh(struct model *model);
void model_set_mesh(struct model *model, struct rsx_mesh *mesh);

struct mat44 model_get_transform_matrix(struct model *model);
