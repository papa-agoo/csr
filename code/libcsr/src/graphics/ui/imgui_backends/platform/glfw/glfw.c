////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui/cimgui.h>
#include <csr/graphics/ui/imgui/cimgui_pimpl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct GLFWwindow GLFWwindow;
extern struct GLFWwindow* g_window;

extern bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window,bool install_callbacks);
extern bool ImGui_ImplGlfw_InitForVulkan(GLFWwindow* window,bool install_callbacks);
extern void ImGui_ImplGlfw_Shutdown();
extern void ImGui_ImplGlfw_NewFrame();
extern void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window,int button,int action,int mods);
extern void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window,double xoffset,double yoffset);
extern void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window,int key,int scancode,int action,int mods);
extern void ImGui_ImplGlfw_CharCallback(GLFWwindow* window,unsigned int c);

////////////////////////////////////////////////////////////

const char* cimgui_platform_get_name()
{
    return "GLFW";
}

result_e cimgui_platform_startup()
{
    bool success = ImGui_ImplGlfw_InitForOpenGL(g_window, true);

    return (success) ? RC_SUCCESS : RC_FAILURE;
}

void cimgui_platform_shutdown()
{
    ImGui_ImplGlfw_Shutdown();
}

void cimgui_platform_new_frame()
{
    ImGui_ImplGlfw_NewFrame();
}
