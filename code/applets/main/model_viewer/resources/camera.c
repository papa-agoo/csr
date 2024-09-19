////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camera.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct camera
{
    struct string name;

    enum camera_projection_mode mode;

    f32 near;
    f32 far;
    f32 fov;
    f32 ortho_size;

    struct transform transform;
};

struct camera* camera_create(struct camera_create_info *info)
{
    check_ptr(info);
    check_expr(info->mode < CAMERA_PROJECTION_MAX);

    ////////////////////////////////////////

    struct camera *camera = calloc(1, sizeof(struct camera));
    check_mem(camera);

    camera->name = (string_is_valid(info->name)) ? info->name : make_string("<no name>");
    camera->mode = info->mode;

    camera_reset(camera);

    ////////////////////////////////////////

    return camera;

error:
    return NULL;
}

void camera_destroy(struct camera* camera)
{
    check_ptr(camera);

    free(camera);

error:
    return;
}

void camera_reset(struct camera *camera)
{
    check_ptr(camera);

    camera->mode = CAMERA_PROJECTION_PERSPECTIVE;

    camera->near = 0.1;
    camera->far = 100.0;
    camera->fov = 60.0;
    camera->ortho_size = 1.0;

    transform_identity(&camera->transform);

error:
    return;
}

struct string camera_get_name(struct camera *camera)
{
    check_ptr(camera);

    return camera->name;

error:
    return make_string("<invalid camera>");
}

enum camera_projection_mode camera_get_projection_mode(struct camera *camera)
{
    check_ptr(camera);

    return camera->mode;

error:
    return CAMERA_PROJECTION_PERSPECTIVE;
}

void camera_set_projection_mode(struct camera *camera, enum camera_projection_mode mode)
{
    check_ptr(camera);
    check_expr(mode < CAMERA_PROJECTION_MAX);

    camera->mode = mode;

error:
    return;
}

f32 camera_get_near_plane(struct camera *camera)
{
    check_expr(camera);

    return camera->near;

error:
    return 0;
}

void camera_set_near_plane(struct camera *camera, f32 near)
{
    check_ptr(camera);

    camera->near = near;

error:
    return;

}

f32 camera_get_far_plane(struct camera *camera)
{
    check_expr(camera);

    return camera->far;

error:
    return 0;
}

void camera_set_far_plane(struct camera *camera, f32 far)
{
    check_ptr(camera);

    camera->far = far;

error:
    return;

}

f32 camera_get_field_of_view(struct camera *camera)
{
    check_expr(camera);

    return camera->fov;

error:
    return 0;
}

void camera_set_field_of_view(struct camera *camera, f32 fov)
{
    check_ptr(camera);
    check_expr(fov > 0);

    camera->fov = fov;

error:
    return;

}

f32 camera_get_ortho_size(struct camera *camera)
{
    check_expr(camera);

    return camera->ortho_size;

error:
    return 0;
}

void camera_set_ortho_size(struct camera *camera, f32 size)
{
    check_ptr(camera);
    check_expr(size > 0);

    camera->ortho_size = size;

error:
    return;

}

struct transform* camera_get_transform(struct camera *camera)
{
    check_ptr(camera);

    return &camera->transform;

error:
    return NULL;
}

void camera_set_transform(struct camera *camera, struct transform *transform)
{
    check_ptr(camera);
    check_ptr(transform);

    camera->transform = *transform;

error:
    return;
}

struct mat44 camera_get_view_matrix(struct camera *camera)
{
    check_ptr(camera);

    return mat44_invert(transform_get_matrix(&camera->transform));

error:
    return mat44_identity();
}

struct mat44 camera_get_projection_matrix(struct camera *camera, f32 aspect)
{
    check_ptr(camera);
    check_expr(aspect > 0);

    if (camera->mode == CAMERA_PROJECTION_ORTHOGRAPHIC)
    {
        f32 a = aspect;
        f32 s = camera->ortho_size;

        return mat44_ortho(-s * a, s * a, -s, s, camera->near, camera->far);
    }

    return mat44_fovy(camera->fov, aspect, camera->near, camera->far);

error:
    return mat44_identity();
}
