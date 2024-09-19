////////////////////////////////////////////////////////////////////////////////////////////////////

#include <model_viewer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _draw_flags_view()
{
    struct mv_renderer_conf *conf = &model_viewer_get_conf()->renderer;

    igCheckbox("Draw Grid", &conf->draw_grid);
    igCheckbox("Draw AABB", &conf->draw_aabb);

    // - draw mode
    //  - point cloud
    //  - wireframe
    //  - solid
    //  - solid lit
    //  - textured
    //  - full
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
    igNewLine();
    _draw_flags_view();

    // igNewLine();
    // igSeparatorText("GPU Renderer");
    // igNewLine();

    // _draw_gpu_renderer_view();

    // igNewLine();
    // igSeparatorText("CPU Renderer");
    // igNewLine();

    // _draw_cpu_renderer_view();

    // igNewLine();
    // igSeparatorText("Settings");
    // igNewLine();

    // _draw_settings_view();
}