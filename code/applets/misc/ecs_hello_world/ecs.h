////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/math.h>
#include <csr/core/math/transform.h>

#include "external/flecs.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

ECS_COMPONENT_EXTERN(TransformComponent);

typedef ecs_world_t* my_scene;

typedef struct
{
    my_scene scene;
    ecs_entity_t entity;
} my_entity;

typedef struct transform TransformComponent;

////////////////////////////////////////////////////////////

my_scene ecs_scene_create();
void ecs_scene_destroy(my_scene scene);

my_entity ecs_mesh_create(my_scene scene, const char *name);

void ecs_mesh_set_position(my_entity mesh, struct vec3 position);
const struct vec3* ecs_mesh_get_position(my_entity mesh);
