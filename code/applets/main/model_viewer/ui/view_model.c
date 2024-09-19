////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_widget_transform(struct transform *transform, bool* show_matrix, f64 dt);

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

    igText("...");
}

static void _draw_detail_view()
{
    // - aabb size original / resized
    // - geometry
    // - materials

    igText("...");
}

static void _draw_transform_view()
{
    struct model *model = model_viewer_get_model();

    // static u32 transform_type = 0;

    // igRadioButton_IntPtr("Global", &transform_type, 0);
    // igSameLine(0, 10);
    // igRadioButton_IntPtr("Local", &transform_type, 1);
    // igNewLine();

    // struct transform *transform = model_get_parent_transform(model);

    // if (transform_type == 1) {
    //     transform = model_get_transform(model);
    // }

    static bool show_model_matrix = false;
    ui_widget_transform(&model->transform, &show_model_matrix, aio_time_elapsed_delta());
}

void ui_draw_model_view()
{
    // igNewLine();
    // igSeparatorText("Load / Import");
    // igNewLine();

    // _draw_load_import_view();

    // igNewLine();
    // igSeparatorText("Details");
    // igNewLine();

    // _draw_detail_view();

    igNewLine();
    igSeparatorText("Transform");
    igNewLine();

    _draw_transform_view();
}
