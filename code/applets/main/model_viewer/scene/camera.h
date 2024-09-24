////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>
#include <csr/core/math/transform.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum camera_projection_mode
{
    CAMERA_PROJECTION_PERSPECTIVE = 0,
    CAMERA_PROJECTION_ORTHOGRAPHIC,

    CAMERA_PROJECTION_MAX,
};

struct camera_create_info
{
    struct string name;

    enum camera_projection_mode mode;
};

struct camera;

struct camera* camera_create(struct camera_create_info *info);
void camera_destroy(struct camera* camera);

void camera_reset(struct camera *camera);

struct string camera_get_name(struct camera *camera);

enum camera_projection_mode camera_get_projection_mode(struct camera *camera);
void camera_set_projection_mode(struct camera *camera, enum camera_projection_mode mode);

f32 camera_get_near_plane(struct camera *camera);
void camera_set_near_plane(struct camera *camera, f32 near);

f32 camera_get_far_plane(struct camera *camera);
void camera_set_far_plane(struct camera *camera, f32 far);

f32 camera_get_field_of_view(struct camera *camera);
void camera_set_field_of_view(struct camera *camera, f32 fov);

f32 camera_get_ortho_size(struct camera *camera);
void camera_set_ortho_size(struct camera *camera, f32 size);

struct transform* camera_get_transform(struct camera *camera);
void camera_set_transform(struct camera *camera, struct transform *transform);

struct mat44 camera_get_view_matrix(struct camera *camera);
struct mat44 camera_get_projection_matrix(struct camera *camera, f32 aspect);
