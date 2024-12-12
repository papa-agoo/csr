////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
// main settings
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _draw_gpu_rsx_view()
{
    struct rsx_conf *conf = mv_rsx_conf_ptr();

    igNewLine();

    if (igCheckbox("Enable##rgpu", &conf->enable_rgpu)) {
        aio_get_ui_window("rgpu")->is_opened = conf->enable_rgpu;
    }

    igNewLine();

    igText("...");

    // - pipeline
    //  - legacy
    //  - pbr

    igNewLine();
}

static void _draw_cpu_rsx_view()
{
    struct rsx_conf *conf = mv_rsx_conf_ptr();

    igNewLine();

    if (igCheckbox("Enable##rcpu", &conf->enable_rcpu)) {
        aio_get_ui_window("rcpu")->is_opened = conf->enable_rcpu;
    }

    igNewLine();

    igText("...");

    // - pipeline
    //  - fixed per vertex
    //  - fixed per pixel
    //  - custom (lua)

    igNewLine();
}

static void _draw_main_settings_view(string_cstr title)
{
    igSeparatorText(title);
    igNewLine();

    ImGuiTreeNodeFlags header_flags = 0;

    if (igCollapsingHeader_TreeNodeFlags("GPU Renderer", header_flags)) {
        _draw_gpu_rsx_view();
    }

    if (igCollapsingHeader_TreeNodeFlags("CPU Renderer", header_flags)) {
        _draw_cpu_rsx_view();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// pass settings
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _draw_pass_environment_view(struct rsx_pass_environment *pass_data)
{
    igNewLine();

    igCheckbox("Enable##pass_env", &pass_data->base.enabled);
    igNewLine();

    igText("...");
    igNewLine();
}

static void _draw_pass_gizmos_view(struct rsx_pass_gizmos *pass_data)
{
    igNewLine();

    igCheckbox("Enable##pass_gizmos", &pass_data->base.enabled);
    igNewLine();

    igCheckbox("Draw Grid", &pass_data->draw_grid);
    igCheckbox("Draw Orientation Axes", &pass_data->draw_orientation_axes);
    igCheckbox("Draw Transform Handles", &pass_data->draw_transform_handles);

    igNewLine();
}

static void _draw_pass_meshes_view(struct rsx_pass_meshes *pass_data)
{
    igNewLine();

    igCheckbox("Enable##pass_meshes", &pass_data->base.enabled);
    igNewLine();

    // - draw mode
    //  - point cloud
    //  - wireframe
    //  - solid
    //  - solid lit
    //  - textured
    //  - full

    igText("...");
    igNewLine();
}

static void _draw_pass_debug_primitives_view(struct rsx_pass_debug_primitives *pass_data)
{
    igNewLine();

    igCheckbox("Enable##pass_debug_primitives", &pass_data->base.enabled);
    igNewLine();

    igText("...");
    igNewLine();
}

static void _draw_pass_settings_view(string_cstr title)
{
    struct rsx_render_data *render_data = rsx_get_render_data();

    igSeparatorText(title);
    igNewLine();

    if (igCollapsingHeader_TreeNodeFlags("Meshes", 0)) {
        _draw_pass_meshes_view(&render_data->pass.meshes);
    }

    if (igCollapsingHeader_TreeNodeFlags("Gizmos", 0)) {
        _draw_pass_gizmos_view(&render_data->pass.gizmos);
    }

    if (igCollapsingHeader_TreeNodeFlags("Environment", 0)) {
        _draw_pass_environment_view(&render_data->pass.environment);
    }

    if (igCollapsingHeader_TreeNodeFlags("Debug Primitives", 0)) {
        _draw_pass_debug_primitives_view(&render_data->pass.debug_primitives);
    }
}

void ui_draw_rsx_view()
{
    igNewLine();
    _draw_main_settings_view("Main Settings");

    igNewLine();
    _draw_pass_settings_view("Pass Settings");
}
