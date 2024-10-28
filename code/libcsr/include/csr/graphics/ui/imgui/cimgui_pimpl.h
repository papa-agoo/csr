////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// platform
struct string cimgui_platform_get_name();

result_e cimgui_platform_startup();
void cimgui_platform_shutdown();

void cimgui_platform_new_frame();


// renderer
struct string cimgui_renderer_get_name();

result_e cimgui_renderer_startup();
void cimgui_renderer_shutdown();

void cimgui_renderer_new_frame(bool update_fonts_texture);
void cimgui_renderer_render_draw_data();
