////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/file_io.h>

#include "model.h"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _process_geometry(cgltf_data *data, struct model *model)
{
    check_ptr(data);
    check_ptr(model);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _process_materials(cgltf_data *data, struct model *model)
{
    check_ptr(data);
    check_ptr(model);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e model_import_gltf(struct model_import_info *info, struct model *model)
{
    check_ptr(info);
    check_ptr(model);

    check_expr(fio_fs_is_file(info->file_path));

    ////////////////////////////////////////

    model->name = string_cut_dir_file(info->file_path).filename;

    cgltf_options options = {0};
    cgltf_data* data = NULL;

    string_cstr path_cstr = string_get_cstr(model->resources.arena, info->file_path);

    check_expr(cgltf_parse_file(&options, path_cstr, &data) == cgltf_result_success);
    check_result(_process_geometry(data, model));
    check_result(_process_materials(data, model));

    cgltf_free(data);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
