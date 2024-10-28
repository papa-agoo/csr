////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui/cimgui.h>
#include <csr/graphics/ui/imgui/cimgui_pimpl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool ImGui_ImplOpenGL3_Init(string_cstr glsl_version);
extern void ImGui_ImplOpenGL3_Shutdown();
extern void ImGui_ImplOpenGL3_NewFrame();
extern void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data);
extern bool ImGui_ImplOpenGL3_CreateFontsTexture();
extern void ImGui_ImplOpenGL3_DestroyFontsTexture();
extern bool ImGui_ImplOpenGL3_CreateDeviceObjects();
extern void ImGui_ImplOpenGL3_DestroyDeviceObjects();

////////////////////////////////////////////////////////////

struct string cimgui_renderer_get_name()
{
    return make_string("OpenGL");
}

result_e cimgui_renderer_startup()
{
    bool success = ImGui_ImplOpenGL3_Init(NULL);

    return (success) ? RC_SUCCESS : RC_FAILURE;
}

void cimgui_renderer_shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
}

void cimgui_renderer_new_frame(bool update_content_scale)
{
    if (update_content_scale)
    {
        ImGui_ImplOpenGL3_DestroyFontsTexture();
        ImGui_ImplOpenGL3_CreateFontsTexture();
    }

    ImGui_ImplOpenGL3_NewFrame();
}

void cimgui_renderer_render_draw_data()
{
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}
