////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/memory/arena.h>

#include "model_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// obj / gltf importer
extern result_e model_import_obj(struct model_import_info *info, struct model *model);
extern result_e model_import_gltf(struct model_import_info *info, struct model *model);

struct model* model_create(struct model_create_info *info)
{
    check_ptr(info);

    struct model *model = calloc(1, sizeof(struct model));
    check_ptr(model);

    model->name = string_is_valid(info->name) ? info->name : make_string("<no name>");

    // FIXME
    model->resources.arena = arena_create(model->name, MegaBytes(32));

    if (info->import)
    {
        struct string file_path = info->import->file_path;

        if (string_has_suffix_cstr(file_path, ".gltf") || string_has_suffix_cstr(file_path, ".glb")) {
            check_result(model_import_gltf(info->import, model));
        }
        else if (string_has_suffix_cstr(file_path, ".obj")) {
            check_result(model_import_obj(info->import, model));
        }
        else {
            check(false, "cannot import model, format not supported : %S", &file_path);
        }
    }

    return model;

error:
    return NULL;
}

void model_destroy(struct model* model)
{
    check_ptr(model);

    arena_destroy(model->resources.arena);

    model->node = (struct mesh_node) {0};

    free(model);

error:
    return;
}
