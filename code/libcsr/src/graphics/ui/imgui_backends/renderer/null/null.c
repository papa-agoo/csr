////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui/cimgui.h>
#include <csr/graphics/ui/imgui/cimgui_pimpl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct string cimgui_renderer_get_name()
{
    return make_string("NULL");
}

result_e cimgui_renderer_startup()
{
    u8* tex_pixels = NULL;
    s32 tex_w, tex_h, tex_bpp;

    ImFontAtlas_GetTexDataAsRGBA32(igGetIO()->Fonts, &tex_pixels, &tex_w, &tex_h, &tex_bpp);

    return RC_SUCCESS;
}

void cimgui_renderer_shutdown() {}

void cimgui_renderer_new_frame(bool update_fonts_texture) {}
void cimgui_renderer_render_draw_data() {}
