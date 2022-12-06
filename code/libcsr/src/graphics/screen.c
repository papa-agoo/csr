////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/screen.h>
#include <csr/kernel/kio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* screen_usage_hint_cstr(enum screen_usage_hint hint)
{
    switch (hint)
    {
        case SCREEN_USAGE_HINT_XGL_API:
            return "SCREEN_USAGE_HINT_XGL_API";

        case SCREEN_USAGE_HINT_NATIVE_API:
            return "SCREEN_USAGE_HINT_NATIVE_API";

        case SCREEN_USAGE_HINT_DIRECT_PIXEL_ACCESS:
            return "SCREEN_USAGE_HINT_DIRECT_PIXEL_ACCESS";

        default:
            return "SCREEN_USAGE_UNKNOWN";
    }
}

const char* screen_usage_hint_cstr_human(enum screen_usage_hint hint)
{
    switch (hint)
    {
        case SCREEN_USAGE_HINT_XGL_API:
            return "XGL API";

        case SCREEN_USAGE_HINT_NATIVE_API:
            return "Native API";

        case SCREEN_USAGE_HINT_DIRECT_PIXEL_ACCESS:
            return "Direct Pixel Access";

        default:
            return "Unknown";
    }
}

////////////////////////////////////////////////////////////////////////////////

struct screen
{
    const char *name;

    u32 width;
    u32 height;

    f32 scale;

    struct {
        struct xgl_viewport viewport;
        struct xgl_clear_values clear_values;

        xgl_texture color_buffer;
        xgl_texture depth_stencil_buffer;

        xgl_framebuffer framebuffer;
    } rt;

    enum screen_usage_hint usage_hint;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _create_render_target(struct screen *screen)
{
    result_e result = RC_FAILURE;

    ////////////////////////////////////////

    struct xgl_viewport viewport = {0};
    viewport.width = screen->width;
    viewport.height = screen->height;
    viewport.min_depth = 0.0f;
    viewport.max_depth = 1.0f;

    screen->rt.viewport = viewport;

    ////////////////////////////////////////

    xgl_texture color_buffer;
    {
        struct xgl_texture_create_info info = {};
        info.type = XGL_TEXTURE_TYPE_2D;
        info.format = XGL_TEXTURE_FORMAT_RGBA;
        info.usage_flags = XGL_TEXTURE_USAGE_COLOR_ATTACHMENT_BIT;
        info.width = screen->width;
        info.height = screen->height;
        info.mip_level_count = 1;
        info.array_layer_count = 1;
        info.sample_count = 1;

        result = xgl_create_texture(&info, &color_buffer);
        check(R_SUCCESS(result), "could not create color buffer");
    }

    screen->rt.color_buffer = color_buffer;

    ////////////////////////////////////////

    xgl_texture depth_stencil_buffer;
    {
        struct xgl_texture_create_info info = {};
        info.type = XGL_TEXTURE_TYPE_2D;
        info.format = XGL_TEXTURE_FORMAT_D24_S8;
        info.usage_flags = XGL_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        info.width = screen->width;
        info.height = screen->height;
        info.mip_level_count = 1;
        info.array_layer_count = 1;
        info.sample_count = 1;

        result = xgl_create_texture(&info, &depth_stencil_buffer);
        check(R_SUCCESS(result), "could not create depth_stencil buffer");
    }

    screen->rt.depth_stencil_buffer = depth_stencil_buffer;

    ////////////////////////////////////////

    xgl_texture attachments[] = {
        color_buffer,
        depth_stencil_buffer,
    };

    struct xgl_framebuffer_create_info info = {0};
    info.width = screen->width;
    info.height = screen->height;
    info.attachments = attachments;
    info.attachment_count = COUNT_OF(attachments);

    return xgl_create_framebuffer(&info, &screen->rt.framebuffer);

error:
    return RC_FAILURE;
}

static void _destroy_render_target(struct screen *screen)
{
    xgl_destroy_texture(screen->rt.color_buffer);
    xgl_destroy_texture(screen->rt.depth_stencil_buffer);
    xgl_destroy_framebuffer(screen->rt.framebuffer);
}

////////////////////////////////////////////////////////////////////////////////

struct screen* screen_create(struct screen_create_info *ci)
{
    check_ptr(ci);

    check_expr(ci->width > 0);
    check_expr(ci->height > 0);
    check_expr(ci->usage_hint != SCREEN_USAGE_HINT_UNKNOWN);

    struct screen *screen = calloc(1, sizeof(struct screen));
    check_mem(screen);

    screen->name = (ci->name) ? strdup(ci->name) : "<unknown>";
    screen->usage_hint = ci->usage_hint;

    screen->width = ci->width;
    screen->height = ci->height;
    screen->scale = (ci->scale > 1) ? ci->scale : 1;

    ////////////////////////////////////////

    struct xgl_clear_values clear_values = {0};
    clear_values.color = ci->clear_color;
    clear_values.depth = 1.0f;
    clear_values.stencil = 0.0f;

    screen->rt.clear_values = clear_values;

    result_e result = _create_render_target(screen);
    check_result(result, "_create_render_target() failed");

    ////////////////////////////////////////

    return screen;

error:
    if (screen) free(screen);

    return NULL;
}

void screen_destroy(struct screen* screen)
{
    check_ptr(screen);

    _destroy_render_target(screen);

    free(screen);

error:
    return;   
}

bool screen_begin(struct screen* screen)
{
    check_ptr(screen);

    // check_expr(screen->is_active);

    struct xgl_render_pass_info pass_info = {};
    pass_info.name = "Screen Pass";
    pass_info.clear_values = screen->rt.clear_values;
    pass_info.framebuffer = screen->rt.framebuffer;

    xgl_set_viewports(1, &screen->rt.viewport);

    return xgl_begin_render_pass(&pass_info);

error:
    return false;
}

void screen_end()
{
    xgl_end_render_pass();

error:
    return;   
}

const char* screen_get_name(struct screen* screen)
{
    check_ptr(screen);

    return screen->name;

error:
    return "<unknown>";
}

enum screen_usage_hint screen_get_usage_hint(struct screen* screen)
{
    check_ptr(screen);

    return screen->usage_hint;

error:
    return SCREEN_USAGE_HINT_UNKNOWN;
}

xgl_texture screen_get_texture(struct screen* screen)
{
    check_ptr(screen);

    return screen->rt.color_buffer;

error:
    return (xgl_texture) {0};
}

void screen_resize(struct screen *screen, u32 width, u32 height)
{
    check_ptr(screen);

    check_expr(width > 0);
    check_expr(height > 0);

    screen->width = width;
    screen->height = height;

    _destroy_render_target(screen);
    _create_render_target(screen);

error:
    return;
}

struct rect screen_get_rect(struct screen *screen)
{
    check_ptr(screen);

    return make_rect(0, 0, screen->width, screen->height);

error:
    return make_rect(0);
}

struct rect screen_get_scaled_rect(struct screen *screen)
{
    check_ptr(screen);

    return make_rect(0, 0, screen->width * screen->scale, screen->height * screen->scale);

error:
    return make_rect(0);
}

void screen_scale_up(struct screen* screen)
{
    check_ptr(screen);

    screen->scale++;

    screen->scale = min(screen->scale, screen_get_max_scale(screen));

error:
    return;
}

void screen_scale_down(struct screen* screen)
{
    check_ptr(screen);

    screen->scale--;

    screen->scale = max(screen->scale, 1);

error:
    return;
}

f32 screen_get_scale(struct screen* screen)
{
    check_ptr(screen);

    return screen->scale;

error:
    return 0;
}

void screen_set_scale(struct screen* screen, f32 factor)
{
    check_ptr(screen);

    check_expr(factor >= 1.0);
    check_expr(factor <= screen_get_max_scale(screen));

    screen->scale = factor;

error:
    return;
}

f32 screen_get_max_scale(struct screen* screen)
{
    check_ptr(screen);

    struct vec2 res = kio_video_get_window_resolution();

    f32 max_scale_w = floorf(res.w / screen->width);
    f32 max_scale_h = floorf(res.h / screen->height);

    return max(max_scale_w, max_scale_h);

error:
    return 0;
}

void screen_maximize_scale(struct screen* screen)
{
    check_ptr(screen);

    screen->scale = screen_get_max_scale(screen);

error:
    return;
}

void screen_reset_scale(struct screen* screen)
{
    check_ptr(screen);

    screen->scale = 1;

error:
    return;
}

bool screen_is_scale_maxed(struct screen* screen)
{
    check_ptr(screen);

    return (screen->scale == screen_get_max_scale(screen));

error:
    return false;
}
