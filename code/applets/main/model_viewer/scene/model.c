////////////////////////////////////////////////////////////////////////////////////////////////////

#include "model_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// obj / gltf importer
extern result_e model_import_obj(struct model_import_info *info, struct model *model);
extern result_e model_import_gltf(struct model_import_info *info, struct model *model);

static result_e _mesh_import_from_file(struct model_import_info *info, struct model *model)
{
    check_ptr(info);
    check_ptr(model);

    struct string file_path = info->file_path;

    ////////////////////////////////////////

    // wavefront obj
    if (string_has_suffix_cstr(file_path, ".obj")) {
        return model_import_obj(info, model);
    }

    // gltf
    if (string_has_suffix_cstr(file_path, ".gltf") || string_has_suffix_cstr(file_path, ".glb")) {
        return model_import_gltf(info, model);
    }

    ////////////////////////////////////////

    clog_warn("cannot import model, format not supported : %S", &file_path);

error:
    return RC_FAILURE;
}

////////////////////////////////////////////////////////////

struct model* model_create(struct model_create_info *info)
{
    check_ptr(info);
    check_ptr(info->arena);

    struct model *model = calloc(1, sizeof(struct model));
    check_ptr(model);

    model->name = string_is_valid(info->name) ? info->name : make_string("<no name>");
    model->priv.arena = info->arena;

    transform_identity(&model->node.transform);

    switch (info->mesh_type)
    {
        case MODEL_MESH_TYPE_TEST_TRIANGLE:
            model_set_mesh(model, rsx_mesh_create_test_triangle());
        break;

        case MODEL_MESH_TYPE_TEST_BOX:
            model_set_mesh(model, rsx_mesh_create_test_box());
        break;

        case MODEL_MESH_TYPE_IMPORT_FROM_FILE:
            check_result(_mesh_import_from_file(&info->import, model));
        break;

        case MODEL_MESH_TYPE_NONE:
        default:
            clog_warn("creating model without a mesh");
        break;
    }

    return model;

error:
    return NULL;
}

void model_destroy(struct model* model)
{
    check_ptr(model);

    if (model->node.mesh) {
        rsx_mesh_destroy(model->node.mesh);
    }

    free(model);

error:
    return;
}

struct rsx_mesh* model_get_mesh(struct model *model)
{
    check_ptr(model);

    return model->node.mesh;

error:
    return NULL;
}

void model_set_mesh(struct model *model, struct rsx_mesh *mesh)
{
    check_ptr(model);

    // release old mesh (lifetime is managed by the resource system)
    if (model->node.mesh) {
        rsx_mesh_destroy(model->node.mesh);
    }

    model->node.mesh = mesh;

    if (mesh && string_is_valid(mesh->name)) {
        model->name = mesh->name;
    }

error:
    return;
}

struct mat44 model_get_transform_matrix(struct model *model)
{
    check_ptr(model);

    struct transform *self = &model->node.transform;

    if (model->node.parent)
    {
        struct transform *parent = &model->node.parent->transform;

        return transform_get_global_matrix(self, parent);
    }

    return transform_get_matrix(self);

error:
    return mat44_identity();
}
