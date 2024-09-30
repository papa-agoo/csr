////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_widget_mat4x4(string_cstr name, const struct mat44* matrix);
void ui_widget_transform(struct transform *transform, bool* show_matrix, f64 dt);

static void _draw_info_view(struct camera *camera)
{
    struct string name = camera_get_name(camera);

    igText("Camera : "string_fmt, string_fmt_arg(name));

    // ...
}

static void _draw_camera_ctl_none()
{
    // ...
}

static void _draw_camera_ctl_orbital(struct camera_ctl_orbital *data)
{
    csr_assert(data);

    // bind widget update rate to time
    f32 update_speed = aio_time_elapsed_delta();

    struct orbit *orbit = &data->orbit_dst;

    igNewLine();

    igDragFloat3("Origin##camera_ctl_orbital", (f32*)&orbit->origin, update_speed, -100.0f, 100.0f, "%.2f", 0);

    igNewLine();

    igDragFloat("Azimuth##camera_ctl_orbital", &orbit->azimuth, update_speed, -360.0, 360.0, "%.3f", 0);
    igDragFloat("Polar##camera_ctl_orbital", &orbit->polar, update_speed, -89.9, 89.9, "%.3f", 0);
    igDragFloat("Radius##camera_ctl_orbital", &orbit->radius, update_speed, 0.5, 100.0, "%.3f", 0);

    igNewLine();

    igDragFloat("Animate##camera_ctl_orbital", &data->animate, update_speed, -50.0f, 50.0f, "%.3f", 0);

    igNewLine();

    igCheckbox("Interpolate##camera_ctl_orbital", &data->interpolate);
    // igDragFloat("Interpolation Scale##camera_ctl_orbital", &data->interpolation_scale, update_speed, 1.0f, 10.0f, "%.3f", 1.0f);

error:
    return;
}

static void _draw_camera_ctl_fps(struct camera_ctl_first_person *data)
{
    igNewLine();
    igText("_draw_camera_ctl_fps()");
}

static struct camera_ctl* _select_camera_controller()
{
    enum camera_ctl_type controller_type = model_viewer_get_camera_ctl()->type;

    // build selectbox
    string_cstr items[] = {"None", "Orbital", "First Person"};
    u32 items_count = COUNT_OF(items);

    if (igCombo_Str_arr("Type##camera.controller", (u32*)&controller_type, items, items_count, items_count)) {
        model_viewer_set_camera_ctl_type(controller_type);
    }

    return model_viewer_get_camera_ctl();
}

static void _draw_controller_view(struct camera *camera)
{
    struct camera_ctl *controller = _select_camera_controller();

    // draw controller ui
    switch (controller->type)
    {
        case CAMERA_CTL_ORBITAL:
            _draw_camera_ctl_orbital(controller->data);
            break;

        case CAMERA_CTL_FIRST_PERSON:
            _draw_camera_ctl_fps(controller->data);
            break;

        default:
            _draw_camera_ctl_none();
    }
}

static void _draw_transform_view(struct camera *camera)
{
    struct transform *transform = camera_get_transform(camera);

    static bool show_view_matrix = false;
    ui_widget_transform(transform, &show_view_matrix, aio_time_elapsed_delta());
}

static void _draw_projection_view(struct camera *camera)
{
    // build selectbox
    string_cstr items[] = {"Perspective", "Orthographic"};
    u32 items_count = COUNT_OF(items);

    enum camera_projection_mode mode = camera_get_projection_mode(camera);

    if (igCombo_Str_arr("Mode##camera.projection", (u32*)&mode, items, items_count, items_count)) {
        camera_set_projection_mode(camera, mode);
    }
    igNewLine();

    ////////////////////////////////////////

    igText("...");
    igNewLine();

    ////////////////////////////////////////

    static bool show_matrix = false;
    igCheckbox("Show Matrix##projection", &show_matrix);

    if (show_matrix)
    {
        struct mat44 matrix = mat44_identity();

        igNewLine();
        ui_widget_mat4x4("Result", &matrix);
    }
}

void ui_draw_camera_view()
{
    struct camera *camera = model_viewer_get_camera();
    csr_assert(camera);

    ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;

    igNewLine();
    _draw_info_view(camera);
    igNewLine();

    if (igCollapsingHeader_TreeNodeFlags("Controller", header_flags))
    {
        igNewLine();
        _draw_controller_view(camera);
        igNewLine();
    }

    if (igCollapsingHeader_TreeNodeFlags("Transform", header_flags))
    {
        igNewLine();
        _draw_transform_view(camera);
        igNewLine();
    }

    if (igCollapsingHeader_TreeNodeFlags("Projection", header_flags))
    {
        igNewLine();
        _draw_projection_view(camera);
        igNewLine();
    }
}
