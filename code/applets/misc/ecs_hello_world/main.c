////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet.h>

#include "ecs.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct string _plugin_get_name()
{
    return make_string("ECS Hello World");
}

static struct string _plugin_get_description()
{
    return make_string("Creates an Enity with a TransformComponent (flecs v2.3.2)");
}

static struct version _plugin_get_version()
{
    return make_version(1, 0, 0);
}

static result_e _plugin_startup()
{
    my_scene g_scene = ecs_scene_create();

    ////////////////////////////////////////

    // create entity and set its position
    my_entity my_mesh = ecs_mesh_create(g_scene, "mesh.cube");
    ecs_mesh_set_position(my_mesh, make_vec3(1, 4, 9));

    // get entity component and print its position
    const struct vec3 *p = ecs_mesh_get_position(my_mesh);
    alog_info("ecs_mesh_get_position() => {%.2f, %.2f, %.2f}\n",  p->x, p->y, p->z);

    ////////////////////////////////////////

    ecs_scene_destroy(g_scene);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void applet_plugin_export(struct applet_plugin* plugin)
{
    plugin->get_name = _plugin_get_name;
    plugin->get_description = _plugin_get_description;
    plugin->get_version = _plugin_get_version;

    plugin->startup = _plugin_startup;
}
