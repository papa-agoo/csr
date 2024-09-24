////////////////////////////////////////////////////////////////////////////////////////////////////

#include "model.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct model
{
    struct string name;

    struct aabb aabb;

    // ...

    struct transform transform;
    struct transform transform_parent;
};

struct model* model_create(struct model_create_info *create_info)
{
    check_ptr(create_info);

    if (string_is_valid(create_info->file_path)) {
        clog_warn("model loading not impl. yet");
    }

    struct model *model = calloc(1, sizeof(struct model));
    check_mem(model);

    model->name = string_is_valid(create_info->name) ? create_info->name : make_string("<no name>");
    model->aabb = make_aabb_unit_cube();

    transform_identity(&model->transform);
    transform_identity(&model->transform_parent);

    return model;

error:
    return NULL;
}

void model_destroy(struct model* model)
{
    check_ptr(model);

    free(model);

error:
    return;
}

struct string model_get_name(struct model *model)
{
    check_ptr(model);

error:
    return make_string("<invalid model>");
}

struct aabb model_get_aabb(struct model* model)
{
    check_ptr(model);

    return model->aabb;

error:
    return make_aabb_zero();
}

struct transform* model_get_transform(struct model *model)
{
    check_ptr(model);

    return &model->transform;

error:
    return NULL;
}

void model_set_transform(struct model *model, struct transform *transform)
{
    check_ptr(model);
    check_ptr(transform);

    clog_warn("model_set_transform() not impl. yet");

error:
    return;
}

struct transform* model_get_parent_transform(struct model *model)
{
    check_ptr(model);

    return &model->transform_parent;

error:
    return NULL;
}

void model_set_parent_transform(struct model *model, struct transform *transform)
{
    check_ptr(model);
    check_ptr(transform);

    clog_warn("model_set_parent_transform() not impl. yet");

error:
    return;
}

struct mat44 model_get_matrix(struct model *model)
{
    check_ptr(model);

    clog_warn("model_get_matrix() not impl. yet");

error:
    return mat44_identity();
}

struct mat44 model_get_world_matrix(struct model *model)
{
    check_ptr(model);

    clog_warn("model_get_world_matrix() not impl. yet");

error:
    return mat44_identity();
}
