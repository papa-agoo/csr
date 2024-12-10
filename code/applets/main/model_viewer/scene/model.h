////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

#include <csr/core/math/aabb.h>
#include <csr/core/math/transform.h>

#include "renderer/common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// model_create(file_path)
//  - resource_load(file_path)
//      - renderer_create_mesh(n)
//      - renderer_create_material(n)


// model
//  - mesh_nodes[]
//      - mesh
//          - mesh_primitives[]


// struct model (gltf style, resource pool [referenced to resource db])
//  - aabb
//  - resources
//      - mesh_buffers[]
//      - meshes[]
//      - mesh_primitives[]
//      - mesh_nodes[]
//      - samplers[]
//      - textures[]
//      - materials[] (effect + params)
//      - ...


// struct mesh_buffer (storage for vertices/indices, gpu + cpu)
//      - vertices
//      - indices
//      - vertex_format
//      - vertex_stride


// struct mesh (spatial grouping of mesh_primitives)
//      - mesh_primitive[]
//      - shader_data (precomputed model matrix, ...)
//      - ...


// struct mesh_primitive (all information needed for xgl_bind_{vertex,index}_buffer())
//      - vertex / index offsets
//      - material
//      - mesh_buffer


// struct mesh_node (used to describe a mesh hiearachy)
//      - aabb
//      - parent node / child nodes
//      - transform
//      - mesh
//      - ...

////////////////////////////////////////////////////////////////////////////////////////////////////

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
