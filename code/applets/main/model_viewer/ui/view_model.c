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

static void _draw_import_model_view(struct model *model)
{
    if (model)
    {
        if (igButton("Unload", make_ImVec2(-1, 0))) {
            model_viewer_unload_model();
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
            struct model_import_info info = {0};
            info.file_path = file_path;
            info.triangulate = settings.triangulate;

            check_result(model_viewer_load_model(file_path));
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

    // _draw_create_procedural_model_view();
    _draw_import_model_view(model);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// model properties
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _draw_detail_view(struct model *model)
{
    igNewLine();
    igText("Name        : %S", &model->name);
    igNewLine();

    // ...
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
        TRANSFORM_TYPE_GLOBAL = 0,
        TRANSFORM_TYPE_LOCAL,
    };

    static enum transform_type_e transform_type = TRANSFORM_TYPE_LOCAL;

    igNewLine();

    igRadioButton_IntPtr("Global", (s32*)&transform_type, 0);
    igSameLine(0, 10);
    igRadioButton_IntPtr("Local", (s32*)&transform_type, 1);

    igNewLine();

    ////////////////////////////////////////

    struct transform *transform = &model->node.transform;

    if (transform_type == TRANSFORM_TYPE_LOCAL) {
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

    if (igCollapsingHeader_TreeNodeFlags("Details", 0)) {
        _draw_detail_view(model);
    }

    if (igCollapsingHeader_TreeNodeFlags("Material", 0)) {
        _draw_material_view(model);
    }

    if (igCollapsingHeader_TreeNodeFlags("Controller", 0)) {
        _draw_controller_view(model);
    }

    if (igCollapsingHeader_TreeNodeFlags("Transform", 0)) {
        _draw_transform_view(model);
    }

error:
    return;
}

void ui_draw_model_view()
{
    struct scene *scene = model_viewer_get_scene();

    igNewLine();
    _draw_load_import_view("Import Settings", scene);

    igNewLine();
    _draw_model_properties_view("Properties", scene);
}
