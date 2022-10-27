////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui/cimgui.h>
#include <csr/graphics/ui/imgui/cimgui_pimpl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* cimgui_renderer_get_name()
{
    return "NULL";
}

result_e cimgui_renderer_startup()
{
    u8* tex_pixels = NULL;
    s32 tex_w, tex_h, tex_bpp;

    ImFontAtlas_GetTexDataAsRGBA32(igGetIO()->Fonts, &tex_pixels, &tex_w, &tex_h, &tex_bpp);

    return RC_SUCCESS;
}

void cimgui_renderer_shutdown() {}

void cimgui_renderer_new_frame() {}
void cimgui_renderer_render_draw_data() {}
