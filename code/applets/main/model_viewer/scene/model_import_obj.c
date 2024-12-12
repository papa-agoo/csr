////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/file_io.h>

#include "model.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <tinyobj_loader_c.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct obj_data
{
    struct arena *arena;

    tinyobj_attrib_t attrib;

    size_t num_shapes;
    tinyobj_shape_t* shapes;

    size_t num_materials;
    tinyobj_material_t* materials;

    u32 flags;
};

static void _read_file_cb(void* ctx, string_cstr filename, const int is_mtl, string_cstr obj_filename, char** data, size_t* byte_length)
{
    struct arena *arena = ctx;

    clog_warn("not impl. yet");

error:
    return;
}

static result_e _import_obj_data(struct obj_data *data, string_cstr filename)
{
    check_ptr(data);
    check_ptr(filename);

    check_expr(tinyobj_parse_obj(&data->attrib, &data->shapes, &data->num_shapes,
        &data->materials, &data->num_materials, filename, _read_file_cb, data->arena, data->flags) == TINYOBJ_SUCCESS);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _process_geometry(struct obj_data *data, struct model *model)
{
    check_ptr(data);
    check_ptr(model);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _process_materials(struct obj_data *data, struct model *model)
{
    check_ptr(data);
    check_ptr(model);

    clog_warn("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e model_import_obj(struct model_import_info *info, struct model *model)
{
    check_ptr(info);
    check_ptr(model);

    check_expr(fio_fs_is_file(info->file_path));

    ////////////////////////////////////////

    model->name = string_cut_dir_file(info->file_path).filename;

    struct obj_data data = {0};
    data.arena = model->resources.arena;
    data.flags = (info->triangulate) ? TINYOBJ_FLAG_TRIANGULATE : 0;

    string_cstr path_cstr = string_get_cstr(model->resources.arena, info->file_path);

    check_result(_import_obj_data(&data,  path_cstr));
    check_result(_process_geometry(&data, model));
    check_result(_process_materials(&data, model));

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
