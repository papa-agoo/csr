////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_widget_mat4x4(string_cstr name, const struct mat44* matrix);
void ui_widget_transform(struct transform *transform, bool* show_matrix, bool invert_matrix, f64 dt);


////////////////////////////////////////////////////////////////////////////////////////////////////
// camera info
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _draw_camera_info_view(string_cstr title, struct scene *scene)
{
    struct camera *camera = scene_get_camera(scene);

    igSeparatorText(title);
    igNewLine();

    igText("Press \"R\" to reset the Camera");

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// camera properties
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _draw_camera_ctl_orbital(struct camera_ctl_orbital *data)
{
    csr_assert(data);

    struct orbit *orbit = &data->orbit_dst;

    enum orbit_orientation orientation = orbit->orientation;
    {
        string_cstr items[] = {"None", "Top", "Bottom", "Front", "Rear", "Left", "Right", "Left Side", "Right Side"};
        u32 items_count = COUNT_OF(items);

        if (igCombo_Str_arr("Orientation##orbital", (u32*)&orientation, items, items_count, items_count)) {
            orbit_set_orientation(orbit, orientation);
        }
    }

    // bind widget update rate to time
    f32 update_speed = aio_time_elapsed_delta();

    igNewLine();
    igDragFloat3("Origin##orbital", (f32*)&orbit->origin, update_speed, -100.0f, 100.0f, "%.2f", 0);

    igNewLine();
    igDragFloat("Azimuth##orbital", &orbit->azimuth, update_speed, -360.0, 360.0, "%.3f", 0);
    igDragFloat("Polar##orbital", &orbit->polar, update_speed, -89.9, 89.9, "%.3f", 0);
    igDragFloat("Radius##orbital", &orbit->radius, update_speed, 0.5, 100.0, "%.3f", 0);

    igNewLine();
    igDragFloat("Animate##orbital", &data->animate, update_speed * (1.0 / 30), -1.0f, 1.0f, "%.3f", 0);

    igNewLine();
    igCheckbox("Interpolate##orbital", &data->interpolate);
}

static void _draw_camera_ctl_fps(struct camera_ctl_first_person *data)
{
    csr_assert(data);

    igNewLine();
    igText("_draw_camera_ctl_fps()");
}

static void _draw_controller_view(struct camera_ctl *camera_ctl)
{
    igNewLine();

    enum camera_ctl_type ctl_type = camera_ctl->type;
    {
        string_cstr items[] = {"None", "Orbital", "First Person"};
        u32 items_count = COUNT_OF(items);

        if (igCombo_Str_arr("Type##camera_ctl", (u32*)&ctl_type, items, items_count, items_count)) {
            model_viewer_set_camera_controller(ctl_type);
        }
    }

    // draw controller ui
    switch (camera_ctl->type)
    {
        case CAMERA_CTL_ORBITAL:
            _draw_camera_ctl_orbital(camera_ctl->data);
        break;

        case CAMERA_CTL_FIRST_PERSON:
            _draw_camera_ctl_fps(camera_ctl->data);
        break;
    }

    igNewLine();
}

static void _draw_transform_view(struct camera *camera)
{
    struct transform *transform = camera_get_transform(camera);

    static bool show_view_matrix = true;

    igNewLine();
    ui_widget_transform(transform, &show_view_matrix, true, aio_time_elapsed_delta());
}

static void _draw_camera_properties_view(string_cstr title, struct scene *scene)
{
    igSeparatorText(title);
    igNewLine();

    ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;

    struct camera *camera = scene_get_camera(scene);
    struct camera_ctl *camera_ctl = scene_get_camera_ctl(scene);

    if (igCollapsingHeader_TreeNodeFlags("Controller", header_flags)) {
        _draw_controller_view(camera_ctl);
    }

    if (igCollapsingHeader_TreeNodeFlags("Transform", header_flags)) {
        _draw_transform_view(camera);
    }

error:
    return;
}

void ui_draw_camera_view()
{
    struct scene *scene = model_viewer_get_scene();

    igNewLine();
    _draw_camera_info_view("Info", scene);

    igNewLine();
    _draw_camera_properties_view("Properties", scene);
}
