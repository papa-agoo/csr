////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vec3.h"
#include "quat.h"
#include "mat4x.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// transform_local_to_world()
// transform_world_to_local()

// transform_{get,set}_rotation()
// transform_rotate()

////////////////////////////////////////////////////////////////////////////////////////////////////

struct transform
{
    struct vec3 position;
    struct quat rotation;
    struct vec3 rotation_euler;
    struct vec3 scale;

    struct mat44 matrix;

    bool is_dirty;
};

void transform_identity(struct transform *transform);

struct vec3 transform_get_position(struct transform *transform);
void transform_set_position(struct transform *transform, struct vec3 position);
void transform_translate(struct transform *transform, struct vec3 delta);

struct quat transform_get_rotation(struct transform *transform);
void transform_set_rotation(struct transform *transform, struct quat q);
void transform_rotate(struct transform *transform, struct quat delta);

struct vec3 transform_get_rotation_euler(struct transform *transform);
void transform_set_rotation_euler(struct transform *transform, struct vec3 angles);
void transform_rotate_euler(struct transform *transform, struct vec3 delta);

struct vec3 transform_get_scale(struct transform *transform);
void transform_set_scale(struct transform *transform, struct vec3 scale);
void transform_scale(struct transform *transform, struct vec3 delta);

void transform_lookat_from(struct transform *transform, struct vec3 position, struct vec3 target);
void transform_lookat(struct transform *transform, struct vec3 target);

struct vec3 transform_local_to_world(struct transform *transform, struct vec3 p);
struct vec3 transform_world_to_local(struct transform *transform, struct vec3 p);

struct mat44 transform_get_matrix(struct transform *transform);
