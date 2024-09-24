////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "model.h"

#include "../renderer/mesh.h"
#include "../renderer/material.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct model
{
    struct string name;

    struct mesh_node node;

    struct {
        // material resources
        struct {
            struct vector *textures;
            struct vector *materials;
        } material;

        // geometry resources
        struct {
            struct vector *meshes;
            struct vector *primitives;
            struct vector *buffers;
            struct vector *nodes;
        } mesh;
    } resources;
};

