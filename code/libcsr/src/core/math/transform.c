////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/math/transform.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void transform_identity(struct transform *transform)
{
    check_ptr(transform);

    transform->position = make_vec3_zero();
    transform->rotation = quat_identity();
    transform->rotation_euler = make_vec3_zero();
    transform->scale = make_vec3_one();

    transform->matrix = mat44_identity();

    transform->is_dirty = false;

error:
    return;
}

struct vec3 transform_get_position(struct transform *transform)
{
    check_ptr(transform);

    return transform->position;

error:
    return make_vec3_zero();
}

void transform_set_position(struct transform *transform, struct vec3 position)
{
    check_ptr(transform);

    transform->position = position;
    transform->is_dirty = true;

error:
    return;
}

void transform_translate(struct transform *transform, struct vec3 delta)
{
    check_ptr(transform);

    transform_set_position(transform, vec3_add(transform->position, delta));

error:
    return;
}

struct quat transform_get_rotation(struct transform *transform)
{
    check_ptr(transform);

    // FIXME need quat impl.

error:
    return quat_identity();
}

void transform_set_rotation(struct transform *transform, struct quat q)
{
    check_ptr(transform);

    // FIXME need quat impl.

error:
    return;
}

void transform_rotate(struct transform *transform, struct quat delta)
{
    check_ptr(transform);

    // FIXME need quat impl.

error:
    return;
}

struct vec3 transform_get_rotation_euler(struct transform *transform)
{
    check_ptr(transform);

    return transform->rotation_euler;

error:
    return make_vec3_zero();
}

void transform_set_rotation_euler(struct transform *transform, struct vec3 rotation)
{
    check_ptr(transform);

    transform->rotation_euler = rotation;
    transform->is_dirty = true;

error:
    return;
}

void transform_rotate_euler(struct transform *transform, struct vec3 delta)
{
    check_ptr(transform);

    transform_set_rotation_euler(transform, vec3_add(transform->rotation_euler, delta));

error:
    return;
}

struct vec3 transform_get_scale(struct transform *transform)
{
    check_ptr(transform);

    return transform->scale;

error:
    return make_vec3_one();
}

void transform_set_scale(struct transform *transform, struct vec3 scale)
{
    check_ptr(transform);

    transform->scale = scale;
    transform->is_dirty = true;

error:
    return;
}

void transform_scale(struct transform *transform, struct vec3 delta)
{
    check_ptr(transform);

    transform_set_scale(transform, vec3_add(transform->scale, delta));

error:
    return;
}

void transform_lookat_from(struct transform *transform, struct vec3 position, struct vec3 target)
{
    check_ptr(transform);
    check_expr(!vec3_equal(position, target));

    // >>> FIXME
    {
        transform->matrix = mat44_invert(mat44_lookat_inv(position, target, make_vec3_up()));
        // transform->matrix = mat44_lookat(position, target, make_vec3_up());

        mat44_decompose(transform->matrix, &transform->position, &transform->rotation_euler, &transform->scale);

        // transform->position = mat44_mult_vec3(transform->matrix, transform->position);
    }
    // <<< FIXME

    transform->is_dirty = false;

error:
    return;
}

void transform_lookat(struct transform *transform, struct vec3 target)
{
    check_ptr(transform);

    transform_lookat_from(transform, transform->position, target);

error:
    return;
}

struct vec3 transform_local_to_world(struct transform *transform, struct vec3 p)
{
    check_ptr(transform);

    // FIXME

error:
    return p;
}

struct vec3 transform_world_to_local(struct transform *transform, struct vec3 p)
{
    check_ptr(transform);

    // FIXME

error:
    return p;
}

struct mat44 transform_get_matrix(struct transform *transform)
{
    check_ptr(transform);

    if (transform->is_dirty)
    {
        transform->matrix = mat44_compose(transform->position, transform->rotation_euler, transform->scale);
        transform->is_dirty = false;

    }

    return transform->matrix;

error:
    return mat44_identity();
}
