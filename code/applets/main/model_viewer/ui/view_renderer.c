////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _draw_flags_view()
{
    struct renderer_conf *conf = mv_renderer_conf_ptr();

    igCheckbox("Draw Grid", &conf->draw_grid);
    igCheckbox("Draw AABB", &conf->draw_aabb);

    igNewLine();

    if (igCheckbox("Enable GPU Renderer", &conf->enable_rgpu)) {
        aio_get_ui_window("rgpu")->is_opened = conf->enable_rgpu;
    }

    if (igCheckbox("Enable CPU Renderer", &conf->enable_rcpu)) {
        aio_get_ui_window("rcpu")->is_opened = conf->enable_rcpu;
    }

    igNewLine();

    // - draw mode
    //  - point cloud
    //  - wireframe
    //  - solid
    //  - solid lit
    //  - textured
    //  - full
error:
    return;
}

static void _draw_gpu_renderer_view()
{
    // - pipeline
    //  - legacy
    //  - pbr

    igText("...");
}

static void _draw_cpu_renderer_view()
{
    // - pipeline
    //  - fixed per vertex
    //  - fixed per pixel
    //  - custom (lua)

    igText("...");
}

static void _draw_settings_view()
{
    igText("...");
}

void ui_draw_renderer_view()
{
    ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;

    igNewLine();
    _draw_flags_view();

    struct renderer_conf *conf = mv_renderer_conf_ptr();

    if (conf->enable_rgpu && igCollapsingHeader_TreeNodeFlags("GPU Renderer", header_flags))
    {
        igNewLine();
        _draw_gpu_renderer_view();
        igNewLine();
    }

    if (conf->enable_rcpu && igCollapsingHeader_TreeNodeFlags("CPU Renderer", header_flags))
    {
        igNewLine();
        _draw_cpu_renderer_view();
        igNewLine();
    }

    // if (igCollapsingHeader_TreeNodeFlags("Settings", header_flags))
    // {
    //     igNewLine();
    //      _draw_settings_view();
    //     igNewLine();
    // }
}
