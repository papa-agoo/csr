////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/memory/arena_priv.h>

#include <csr/graphics/ui/imgui/cimgui_priv.h>
#include <csr/graphics/ui/imgui/cimgui_pimpl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct cimgui_info g_backend_info = {
    .platform_name = make_string_static("NONE"),
    .renderer_name = make_string_static("NONE"),
};

const struct cimgui_info* cimgui_get_info()
{
    return &g_backend_info;
}

result_e cimgui_init(struct string ini_file)
{
    check_expr(string_is_valid(ini_file));

    ////////////////////////////////////////

    igCreateContext(NULL);

    ImGuiContext *ctx = igGetCurrentContext();

    // config
    ctx->IO.ConfigWindowsMoveFromTitleBarOnly = true;
    ctx->IO.ConfigDockingWithShift = true;

    // config flags
    ctx->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // misc
    ctx->IO.IniFilename = string_get_cstr(_arena_priv_ptr(), ini_file);

    ////////////////////////////////////////

    check_expr(cimgui_platform_startup() == RC_SUCCESS);
    check_expr(cimgui_renderer_startup() == RC_SUCCESS);

    g_backend_info.platform_name = cimgui_platform_get_name();
    g_backend_info.renderer_name = cimgui_renderer_get_name();

    return RC_SUCCESS;

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
