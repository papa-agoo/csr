////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_widget_transform(struct transform *transform, bool* show_matrix, bool invert_matrix, f64 dt);

static void _os_open_file_dialog()
{
    // zenity --file-selection --filename=/opt/csr/resources/xxx/
    //      images: --file-filter="Image Files | *.png *.jpg *.jpeg *.xxx"
    //      models: --file-filter="3D Models (*.obj, *.glTF) | *.obj *.gltf *.glb"

    // popen(...)
}

static void _draw_load_import_view()
{
    // - load / import
    //      - none
    //          - <empty scene>
    //      - builtin
    //          - cube
    //          - sphere
    //          - torus
    //          - ...
    //      - load from file
    //          - bool: fit into frustum
    //          - vec3: placement / pivot

    // tools
    //      - zenity file dialog
    //      - cgltf library
    //      - tinyobjloader-c library

    igText("...");
}

static void _draw_detail_view(struct model *model)
{
    // - aabb size original / resized
    // - meshes
    // - materials

    struct string name = model_get_name(model);
    igText("Model : %S", &name);

    // - [ ] draw aabb
    // - [ ] draw parent aabb
}

static void _draw_transform_view(struct model* model)
{
    static enum {
        TRANSFORM_TYPE_GLOBAL = 0,
        TRANSFORM_TYPE_LOCAL,
    } transform_type;

    igRadioButton_IntPtr("Global", (s32*)&transform_type, 0);
    igSameLine(0, 10);
    igRadioButton_IntPtr("Local", (s32*)&transform_type, 1);

    igNewLine();

    ////////////////////////////////////////

    struct transform *transform = model_get_transform(model);

    // struct transform *transform = model_get_parent_transform(model);

    // if (transform_type == TRANSFORM_TYPE_LOCAL) {
    //     transform = model_get_transform(model);
    // }

    ////////////////////////////////////////

    // static bool show_model_matrix = true;
    // ui_widget_transform(transform, &show_model_matrix, false, aio_time_elapsed_delta());
}

void ui_draw_model_view()
{
    ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;

    igNewLine();

    if (igCollapsingHeader_TreeNodeFlags("Load / Import", header_flags))
    {
        igNewLine();
        _draw_load_import_view();
        igNewLine();
    }

    struct model *model = model_viewer_get_model();
    check_quiet(model);

    if (igCollapsingHeader_TreeNodeFlags("Details", header_flags))
    {
        igNewLine();
        _draw_detail_view(model);
        igNewLine();
    }

    if (igCollapsingHeader_TreeNodeFlags("Transform", header_flags))
    {
        igNewLine();
        _draw_transform_view(model);
        igNewLine();
    }


error:
    return;
}
