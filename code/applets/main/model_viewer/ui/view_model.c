////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_widget_transform(struct transform *transform, bool* show_matrix, bool invert_matrix, f64 dt);


////////////////////////////////////////////////////////////////////////////////////////////////////
// load / import
////////////////////////////////////////////////////////////////////////////////////////////////////
#define MODEL_RESOURCE_DIR "{RESOURCE_DIR}/models/"
#define MODEL_PATH_LENGTH 256

// FIXME zenity seems to like absolute path more
static struct string _open_file_dialog(struct string workdir)
{
    struct arena *arena = aio_get_main_arena();

    // build cmd string
    struct string cmd = string_create_fmt(arena,
        "zenity --file-selection --filename=%S --file-filter='%s | %s' 2> /dev/null",
        &workdir,
        "3D Models",
        "*.obj *.gltf *.glb"
    );

    // exec cmd
    void *path_cstr = arena_push(arena, MODEL_PATH_LENGTH);
    {
        FILE *fp = popen(cmd.ptr, "r");
        check(fp, "could not exec cmd : %S", &cmd);

        fgets(path_cstr, MODEL_PATH_LENGTH, fp);
        fclose(fp);
    }

    return string_strip(make_string_from_cstr(path_cstr));

error:
    return make_string_invalid();
}

static void _draw_load_procedural_mesh_view(struct model *model)
{
    enum procedural_mesh_type {
        PROCEDURAL_MESH_TYPE_NONE = MODEL_MESH_TYPE_NONE,
        PROCEDURAL_MESH_TYPE_TRIANGLE = MODEL_MESH_TYPE_TEST_TRIANGLE,
        PROCEDURAL_MESH_TYPE_BOX = MODEL_MESH_TYPE_TEST_BOX,
    };

    static enum procedural_mesh_type mesh_type = PROCEDURAL_MESH_TYPE_TRIANGLE;
    {
        string_cstr items[] = {"None", "Test Triangle", "Test Box"};
        u32 items_count = COUNT_OF(items);

        if (model == NULL || igCombo_Str_arr("Mesh##model", (u32*)&mesh_type, items, items_count, items_count))
        {
            struct model_create_info model_info = {0};
            model_info.arena = model_viewer_get_resource_arena();
            model_info.mesh_type = (enum model_mesh_type) mesh_type;

            model_viewer_set_model(model_create(&model_info));
        }
    }

    // enum material_type {
    //     MATERIAL_TYPE_NONE = 0,
    //     MATERIAL_TYPE_POINTS,
    //     MATERIAL_TYPE_WIREFRAME,
    //     MATERIAL_TYPE_DEBUG_COLORS,
    //     MATERIAL_TYPE_DEBUG_NORMALS,
    //     MATERIAL_TYPE_DEBUG_TEXCOORDS,
    // };

    // static enum material_type material_type = 0;

    // if (mesh_type != PROCEDURAL_MESH_TYPE_NONE)
    // {
    //     string_cstr items[] = {"Points", "Wireframe", "Debug Colors", "Debug Normals", "Debug Texcoords"};
    //     u32 items_count = COUNT_OF(items);

    //     if (igCombo_Str_arr("Material##model", (u32*)&material_type, items, items_count, items_count))
    //     {
    //         // ...
    //     }
    // }
}

static void _draw_import_model_view(struct model *model)
{
    igNewLine();

    if (model)
    {
        if (igButton("Unload", make_ImVec2(-1, 0))) {
            model_viewer_set_model(NULL);
        }

        return;
    }

    ////////////////////////////////////////

    struct import_settings {
        bool autofit;
        bool autoplace;
        bool triangulate;
    };

    static struct import_settings settings = {0};
    {
        igCheckbox("Autofit", &settings.autofit);
        igCheckbox("Autoplace", &settings.autoplace);
        igCheckbox("Triangulate", &settings.triangulate);
        igNewLine();

        if (igButton("Load File", make_ImVec2(-1, 0)))
        {
            // select file
            struct string file_path = _open_file_dialog(aio_env_expand_str(MODEL_RESOURCE_DIR));
            check_expr(string_is_valid(file_path));

            // load model
            alog_notice("loading model : %S", &file_path);
            {
                struct model_create_info model_info = {0};
                model_info.arena = model_viewer_get_resource_arena();
                model_info.mesh_type = MODEL_MESH_TYPE_IMPORT_FROM_FILE;
                model_info.import.file_path = file_path;
                model_info.import.triangulate = settings.triangulate;

                model_viewer_set_model(model_create(&model_info));
            }
        }
    }

error:
    return;
}

static void _draw_load_import_view(string_cstr title, struct scene *scene)
{
    struct model *model = scene_get_model(scene);

    igSeparatorText(title);
    igNewLine();

    enum model_source {
        MODEL_SOURCE_NONE = 0,

        MODEL_SOURCE_LOAD_PROCEDURAL,
        MODEL_SOURCE_IMPORT_FROM_FILE,
    };

    static enum model_source model_source = MODEL_SOURCE_NONE;

    string_cstr items[] = {"None", "Procedural Mesh", "Import from File"};
    u32 items_count = COUNT_OF(items);

    if (igCombo_Str_arr("Mesh Source##model", (u32*)&model_source, items, items_count, items_count)) {
        model_viewer_set_model(NULL);
    }

    switch (model_source)
    {
        case MODEL_SOURCE_LOAD_PROCEDURAL:
            _draw_load_procedural_mesh_view(model);
        break;

        case MODEL_SOURCE_IMPORT_FROM_FILE:
            _draw_import_model_view(model);
        break;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// model properties
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _draw_detail_view(struct model *model)
{
    igNewLine();
    igText("Name            : %S", &model->name);

    ////////////////////////////////////////

    struct rsx_mesh *mesh = model_get_mesh(model);

    if (mesh)
    {
        struct rsx_mesh_info *info = &mesh->info;

        igNewLine();
        igText("Meshes          : %d", info->primitive_count);
        igText("Materials       : %d", 0);

        igNewLine();
        igText("Vertices        : %d", info->vertices_count);
        igText("Vertex Buffer   : %d Bytes", info->vertices_byte_length);

        igNewLine();
        igText("Indices         : %d", info->indices_count);
        igText("Index Buffer    : %d Bytes", info->indices_byte_length);

        // FIXME human readable sizes
        // - 16 Bytes       => 16 Bytes -- no tooltip
        // - 1024 Bytes     => 1 KiB (?) => 1024 Bytes
        // - 1048576 Bytes  => 1 MiB (?) => 1048576 Bytes

        igNewLine();
        igText("Memory Used     : %d Bytes (%.2f KiB)", info->total_byte_length, info->total_byte_length / 1024.0);
    }
    else
    {
        // FIXME get color from global config
        igNewLine();
        igTextColored(make_ImVec4(0.85f, 0.45f, 0.25f, 1.00f), "Model has no Mesh");
    }

    ////////////////////////////////////////

    igNewLine();

error:
    return;
}

static void _draw_material_view(struct model *model)
{
    igNewLine();
    igText("...");

    igNewLine();
}

static void _draw_controller_view(struct model *model)
{
    igNewLine();
    igText("...");

    igNewLine();
}

static void _draw_transform_view(struct model* model)
{
    enum transform_type_e {
        TRANSFORM_TYPE_PARENT = 0,
        TRANSFORM_TYPE_SELF,
    };

    static enum transform_type_e transform_type = TRANSFORM_TYPE_SELF;

    igNewLine();

    igRadioButton_IntPtr("Parent", (s32*)&transform_type, 0);
    igSameLine(0, 10);
    igRadioButton_IntPtr("Self", (s32*)&transform_type, 1);

    igNewLine();

    ////////////////////////////////////////

    struct transform *transform = &model->node.transform;

    if (transform_type == TRANSFORM_TYPE_PARENT) {
        transform = &model->node.parent->transform;
    }

    ////////////////////////////////////////

    static bool show_model_matrix = true;
    ui_widget_transform(transform, &show_model_matrix, false, aio_time_elapsed_delta());

    igNewLine();
}

static void _draw_model_properties_view(string_cstr title, struct scene *scene)
{
    struct model *model = scene_get_model(scene);
    check_quiet(model);

    igSeparatorText(title);
    igNewLine();

    ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;

    if (igCollapsingHeader_TreeNodeFlags("Details", header_flags)) {
        _draw_detail_view(model);
    }

    if (igCollapsingHeader_TreeNodeFlags("Material", 0)) {
        _draw_material_view(model);
    }

    if (igCollapsingHeader_TreeNodeFlags("Controller", 0)) {
        _draw_controller_view(model);
    }

    if (igCollapsingHeader_TreeNodeFlags("Transform", header_flags)) {
        _draw_transform_view(model);
    }

error:
    return;
}

void ui_draw_model_view()
{
    struct scene *scene = model_viewer_get_scene();

    igNewLine();
    _draw_load_import_view("Load / Import Settings", scene);

    igNewLine();
    _draw_model_properties_view("Properties", scene);
}
