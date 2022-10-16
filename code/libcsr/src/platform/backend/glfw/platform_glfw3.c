////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/platform/platform_pimpl.h>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

GLFWwindow* g_window;

static struct platform_callbacks *g_platform_callbacks;

extern enum keyboard_key get_csr_kbd_key(s32 glfw_key);
extern s32 get_glfw_kbd_key(enum keyboard_key key);

extern enum mouse_button get_csr_mouse_button(s32 glfw_button);
extern s32 get_glfw_mouse_button(enum mouse_button button);


////////////////////////////////////////////////////////////////////////////////
// platform callbacks
////////////////////////////////////////////////////////////////////////////////
static void error_callback(s32 error, const char* msg)
{
    g_platform_callbacks->on_error(error, msg);
}

static void _key_input_cb(GLFWwindow* window, s32 key, s32 code, s32 action, s32 mods)
{
    g_platform_callbacks->on_keyboard_key_change(
        get_csr_kbd_key(key), (action == GLFW_PRESS));
}

static void _win_close_cb(GLFWwindow* window)
{
    if (!glfwWindowShouldClose(window)) {
        return;
    }

    g_platform_callbacks->on_window_close();
}

static void _window_size_callback(GLFWwindow* window, s32 w, s32 h)
{
    g_platform_callbacks->on_window_resize(w, h);
}

static void _mouse_move_callback(GLFWwindow* window, f64 x, f64 y)
{
    g_platform_callbacks->on_mouse_move(x, y);
}

static void _mouse_wheel_callback(GLFWwindow* window, f64 x, f64 y)
{
    g_platform_callbacks->on_mouse_wheel_change(x, y);
}

static void _mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods)
{
    g_platform_callbacks->on_mouse_button_change(
        get_csr_mouse_button(button), (action == GLFW_PRESS));
}


////////////////////////////////////////////////////////////////////////////////
// platform api impl
////////////////////////////////////////////////////////////////////////////////
static void _enumerate_monitors()
{
    // FIXME

    // list connected monitors and their native resolutions
    s32 num_monitors;
    GLFWmonitor** monitors = glfwGetMonitors(&num_monitors);

    for (s32 monitor_id = 0; monitor_id < num_monitors; monitor_id++)
    {
        GLFWmonitor* monitor = monitors[monitor_id];
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // ("Monitor %d: %s (%dx%d @ %dHz)",
        //         monitor_id,
        //         glfwGetMonitorName(monitor),
        //         mode->width,
        //         mode->height,
        //         mode->refreshRate);
    }
}

result_e platform_init_impl(struct platform_init_info *init_info, struct platform_callbacks *callbacks)
{
    check_ptr(init_info);
    check_ptr(callbacks);
    g_platform_callbacks = callbacks;

    if (!glfwInit()) return RC_FAILURE;

    ////////////////////////////////////////

    // create window and opengl context
    glfwWindowHint(GLFW_SAMPLES, init_info->hint.sample_count);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, init_info->hint.debug_context);

    g_window = glfwCreateWindow(init_info->width, init_info->height, init_info->title, NULL, NULL);
    check(g_window != NULL, "could not create window");

    glfwMakeContextCurrent(g_window);

    ////////////////////////////////////////

    // set callbacks
    glfwSetWindowCloseCallback(g_window, _win_close_cb);
    glfwSetWindowSizeCallback(g_window, _window_size_callback);

    glfwSetKeyCallback(g_window, _key_input_cb);

    glfwSetCursorPosCallback(g_window, _mouse_move_callback);
    glfwSetScrollCallback(g_window, _mouse_wheel_callback);
    glfwSetMouseButtonCallback(g_window, _mouse_button_callback);

    //////////////////////////////////////// 

    // apply current settings
    platform_win_enable_vsync_impl(init_info->enable_vsync);
    platform_win_enable_fullscreen_impl(init_info->fullscreen);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:

    glfwTerminate();

    return RC_FAILURE;
}

void platform_quit_impl()
{
    if (g_window) {
         glfwDestroyWindow(g_window);
    }

    glfwTerminate();
}

void platform_get_info_impl(struct platform_info *info)
{
    check_ptr(info);

    ////////////////////////////////////////

    // provide display info
    struct platform_display_info *display = &info->display;
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();

        // display name
        display->name = strdup(glfwGetMonitorName(monitor));

        // resolution in pixels
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        display->resolution = make_vec2(mode->width, mode->height);

        // physical size in mm
        s32 width_mm, height_mm;

        glfwGetMonitorPhysicalSize(monitor, &width_mm, &height_mm);
        display->physical_size = make_vec2(width_mm, height_mm);

        // dpi aware content scale
        glfwGetWindowContentScale(g_window, &display->content_scale, NULL);
    }

    ////////////////////////////////////////

    // provide backend info
    struct platform_backend_info *backend = &info->backend;
    {
        backend->name = strdup("GLFW");
        backend->type = PLATFORM_BACKEND_GLFW3;

        struct version v = {0};
        glfwGetVersion(&v.major, &v.minor, &v.micro);

        backend->version = v;
    }

    ////////////////////////////////////////

    // provide renderer info
    struct platform_renderer_info *renderer = &info->renderer;
    {
        renderer->name = strdup("OpenGL");
        renderer->type = PLATFORM_RENDERER_OPENGL;

        // opengl version
        struct version v = {0};
        glGetIntegerv(GL_MAJOR_VERSION, &v.major);
        glGetIntegerv(GL_MINOR_VERSION, &v.minor);

        renderer->version = v;

        // glsl version
        renderer->version_shading_lang = glGetString(GL_SHADING_LANGUAGE_VERSION);

        // sample count (MSAA)
        glGetIntegerv(GL_SAMPLES, &renderer->sample_count);
        glGetIntegerv(GL_MAX_SAMPLES, &renderer->sample_count_max);

        // debug context
        s32 context_flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);

        renderer->debug_context_enabled = (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT);
    }

error:
    return;
}

void platform_swap_buffers_impl()
{
    glfwSwapBuffers(g_window);
}

void platform_poll_events_impl()
{
    glfwPollEvents();
}

f64 platform_time_elapsed_impl()
{
    return glfwGetTime();
}

bool platform_keyboard_key_down_impl(enum keyboard_key key)
{
    s32 glfw_key = get_glfw_kbd_key(key);

    return (glfwGetKey(g_window, glfw_key) == GLFW_PRESS);
}

bool platform_mouse_button_down_impl(enum mouse_button button)
{
    s32 glfw_button = get_glfw_mouse_button(button);

    return (glfwGetMouseButton(g_window, glfw_button) == GLFW_PRESS);
}

void platform_win_resize_impl(u32 width, u32 height)
{
    glfwSetWindowSize(g_window, width, height);
}

void platform_win_enable_vsync_impl(bool enable)
{
    glfwSwapInterval((enable) ? 1 : 0);
}

void platform_win_enable_fullscreen_impl(bool enable)
{
    static s32 x_windowed = 0;
    static s32 y_windowed = 0;

    static u32 w_windowed = 0;
    static u32 h_windowed = 0;

    ////////////////////////////////////////

    GLFWmonitor* monitor = NULL;

    s32 x = x_windowed;
    s32 y = y_windowed;

    u32 w = w_windowed;
    u32 h = h_windowed;

    s32 refresh_rate = GLFW_DONT_CARE;

    ////////////////////////////////////////

    // window -> fullscreen
    if (enable)
    {
        // save window position
        glfwGetWindowPos(g_window, &x_windowed, &y_windowed);

        // save framebuffer size
        glfwGetFramebufferSize(g_window, &w_windowed, &h_windowed);

        // tell glfw to go fullscreen
        monitor = glfwGetPrimaryMonitor();

        // set fullscreen video mode to current desktop settings (resolution, refresh rate)
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        w = mode->width;
        h = mode->height;

        refresh_rate = mode->refreshRate;
    }

    ////////////////////////////////////////

    // set video mode
    glfwSetWindowMonitor(g_window, monitor, x, y, w, h, refresh_rate);

    // reset window state (maximized, minized, ...)
    glfwRestoreWindow(g_window);
}

void platform_win_enable_mouse_capture_impl(bool enable)
{
    s32 cursor_state = (enable) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;

    glfwSetInputMode(g_window, GLFW_CURSOR, cursor_state);
}
