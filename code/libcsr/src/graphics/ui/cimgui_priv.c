////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui/cimgui_priv.h>
#include <csr/graphics/ui/imgui/cimgui_pimpl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct cimgui_info g_backend_info = {
    .platform_name = "NONE",
    .renderer_name = "NONE",
};

const struct cimgui_info* cimgui_get_info()
{
    return &g_backend_info;
}

result_e cimgui_init(const char *ini_file)
{
    check_ptr(ini_file);

    ////////////////////////////////////////

    igCreateContext(NULL);

    ImGuiContext *ctx = igGetCurrentContext();
    ctx->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ctx->IO.ConfigWindowsMoveFromTitleBarOnly = true;
    ctx->IO.IniFilename = strdup(ini_file);

    ////////////////////////////////////////

    result_e result = RC_FAILURE;

    result = cimgui_platform_startup();
    check_result(result, "cimgui_platform_startup() failed ...");

    result = cimgui_renderer_startup();
    check_result(result, "cimgui_renderer_startup() failed ...");

    g_backend_info.platform_name = strdup( cimgui_platform_get_name() );
    g_backend_info.renderer_name = strdup( cimgui_renderer_get_name() );

    return result;

error:
    return RC_FAILURE;
}

void cimgui_quit()
{
    cimgui_renderer_shutdown();
    cimgui_platform_shutdown();

    igDestroyContext(igGetCurrentContext());
}

void cimgui_frame_begin(bool update_content_scale)
{
    cimgui_renderer_new_frame(update_content_scale);
    cimgui_platform_new_frame();

    igNewFrame();
}

void cimgui_frame_end()
{
    igRender();

    cimgui_renderer_render_draw_data();
}
