////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet/aio.h>

#include "scene_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _build_model_cache(struct scene_cache *cache);
static result_e _build_camera_cache(struct scene_cache *cache);

result_e scene_init(struct scene *scene)
{
    check_ptr(scene);

    check_result(_build_model_cache(&scene->cache));
    check_result(_build_camera_cache(&scene->cache));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void scene_quit(struct scene *scene)
{
    check_ptr(scene);

    // ...

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// models
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _build_model_cache(struct scene_cache *cache)
{
    check_ptr(cache);

    // create dummy model
    {
        struct model_create_info create_info = {0};
        create_info.name = make_string("Dummy");

        struct model *model = model_create(&create_info);
        check_ptr(model);

        cache->model.dummy = model;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// cameras
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _camera_ctl_orbital_update_cb(struct camera *camera, struct camera_ctl *ctl)
{
    check_ptr(camera);
    check_ptr(ctl);

    check_expr(ctl->type == CAMERA_CTL_ORBITAL);

    struct camera_ctl_orbital *data = ctl->data;
    check_ptr(data);

    struct transform *transform = camera_get_transform(camera);
    check_ptr(transform);

    struct orbit *orbit = &data->orbit_src;

    orbit_set_azimuth(&data->orbit_dst, data->orbit_dst.azimuth + data->animate);

    if (data->interpolate)
    {
        f32 scale = 6.0; // FIXME move to config
        f32 value = scale * aio_time_elapsed_delta();

        struct orbit *src = &data->orbit_src;
        struct orbit *dst = &data->orbit_dst;

        src->origin = vec3_lerp(src->origin, dst->origin, value);
        src->azimuth = lerp(value, src->azimuth, dst->azimuth);
        src->polar = lerp(value, src->polar, dst->polar);
        src->radius = lerp(value, src->radius, dst->radius);
    }
    else
    {
        data->orbit_src = data->orbit_dst;
    }

    transform_lookat_from(transform, orbit_get_coords(orbit), orbit->origin);

error:
    return;
}

static void _camera_ctl_first_person_update_cb(struct camera *camera, struct camera_ctl *ctl)
{
    // ...
}

static result_e _build_camera_cache(struct scene_cache *cache)
{
    check_ptr(cache);

    // create main camera
    {
        struct camera_create_info create_info = {0};
        create_info.name = make_string("main");
        create_info.mode = CAMERA_PROJECTION_PERSPECTIVE;

        struct camera *camera = camera_create(&create_info);
        check(camera, "could not create main camera");

        cache->camera.main = camera;
    }

    // create dummy controller
    {
        struct camera_ctl ctl = {0};
        ctl.type = CAMERA_CTL_NONE;

        cache->camera.controller.none = ctl;
    }

    // create orbital controller
    {
        f32 azimuth = 75.0;
        f32 polar = 15.0;
        f32 radius = 100.0;

        static struct camera_ctl_orbital data = {0};
        data.orbit_dst.azimuth = azimuth;
        data.orbit_dst.polar = polar;
        data.orbit_dst.radius = radius;
        data.orbit_src = data.orbit_dst;
        data.interpolate = true;

        struct camera_ctl ctl = {0};
        ctl.type = CAMERA_CTL_ORBITAL;
        ctl.update_cb = _camera_ctl_orbital_update_cb;
        ctl.data = &data;

        cache->camera.controller.orbital = ctl;
    }

    // create first person controller
    {
        struct camera_ctl ctl = {0};
        ctl.type = CAMERA_CTL_FIRST_PERSON;
        ctl.update_cb = _camera_ctl_first_person_update_cb;

        // ...

        cache->camera.controller.first_person = ctl;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
