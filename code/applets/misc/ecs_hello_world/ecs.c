////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ecs.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

ECS_TAG_DECLARE(Mesh);

ECS_COMPONENT_DECLARE(TransformComponent);

my_scene ecs_scene_create()
{
    my_scene scene = ecs_init();
    check_ptr(scene);

    ECS_TAG_DEFINE(scene, Mesh);

    ECS_COMPONENT_DEFINE(scene, TransformComponent);

    return scene;

error:
    return NULL;
}

void ecs_scene_destroy(my_scene scene)
{
    ecs_fini(scene);
}

my_entity ecs_mesh_create(my_scene scene, const char *name)
{
    ecs_entity_t entity = ecs_new(scene, Mesh);

    ecs_set(scene, entity, EcsName, { .value = name });

    ecs_add(scene, entity, TransformComponent);
    ecs_set(scene, entity, TransformComponent, {0});

    my_entity mesh = {0};
    mesh.scene = scene;
    mesh.entity = entity;

    return mesh;

error:
    return (my_entity){0};
}

void ecs_mesh_set_position(my_entity e, struct vec3 position)
{
    check_expr(ecs_has(e.scene, e.entity, TransformComponent));

    TransformComponent *t = ecs_get_mut(e.scene, e.entity, TransformComponent, NULL);
    check_ptr(t);

    t->position = position;
    t->is_dirty = true;

error:
    return;
}

const struct vec3* ecs_mesh_get_position(my_entity mesh)
{
    return &ecs_get(mesh.scene, mesh.entity, TransformComponent)->position;
}
