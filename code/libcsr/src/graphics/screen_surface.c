////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/screen_surface.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct screen_surface
{
    enum screen_surface_type type;

    struct xgl_viewport viewport;
    struct xgl_clear_values clear_values;

    struct pixelbuffer pb;

    xgl_texture color_buffer;
    xgl_texture depth_stencil_buffer;

    xgl_framebuffer framebuffer;
};

static void _destroy_buffers(struct screen_surface *surface)
{
    check_ptr(surface);

    if (guid_valid(surface->color_buffer.handle)) {
        xgl_destroy_texture(surface->color_buffer);
    }

    if (guid_valid(surface->depth_stencil_buffer.handle)) {
        xgl_destroy_texture(surface->depth_stencil_buffer);
    }

    if (guid_valid(surface->framebuffer.handle)) {
        xgl_destroy_framebuffer(surface->framebuffer);
    }

error:
    return;
}

static result_e _create_buffers(struct screen_surface *surface)
{
    check_ptr(surface);

    struct vec2 size = make_vec2(surface->viewport.width, surface->viewport.height);

    ////////////////////////////////////////

    enum xgl_texture_usage stream_flags = 0;

    if (surface->type == SCREEN_SURFACE_TYPE_CPU)
    {
        struct pixelbuffer *pb = &surface->pb;

        pb->width = size.w;
        pb->height = size.h;
        pb->num_pixels = size.w * size.h;

        // FIXME write only => massive performance hit?
        stream_flags = XGL_TEXTURE_USAGE_STREAM_READ_BIT | XGL_TEXTURE_USAGE_STREAM_WRITE_BIT;
    }

    ////////////////////////////////////////

    xgl_texture color_buffer;
    {
        struct xgl_texture_create_info info = {};
        info.type = XGL_TEXTURE_TYPE_2D;
        info.format = XGL_TEXTURE_FORMAT_RGBA;
        info.usage_flags = XGL_TEXTURE_USAGE_COLOR_ATTACHMENT_BIT | stream_flags;
        info.width = size.w;
        info.height = size.h;
        info.mip_level_count = 1;
        info.array_layer_count = 1;
        info.sample_count = 1;

        result_e result = xgl_create_texture(&info, &color_buffer);
        check_result(result, "could not create color buffer");
    }

    surface->color_buffer = color_buffer;

    ////////////////////////////////////////

    xgl_texture depth_stencil_buffer;
    {
        struct xgl_texture_create_info info = {};
        info.type = XGL_TEXTURE_TYPE_2D;
        info.format = XGL_TEXTURE_FORMAT_D24_S8;
        info.usage_flags = XGL_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | stream_flags;
        info.width = size.w;
        info.height = size.h;
        info.mip_level_count = 1;
        info.array_layer_count = 1;
        info.sample_count = 1;

        result_e result = xgl_create_texture(&info, &depth_stencil_buffer);
        check_result(result, "could not create depth_stencil buffer");
    }

    surface->depth_stencil_buffer = depth_stencil_buffer;

    ////////////////////////////////////////

    xgl_framebuffer framebuffer;
    {
        xgl_texture attachments[] = {
            color_buffer,
            depth_stencil_buffer,
        };

        struct xgl_framebuffer_create_info info = {0};
        info.width = size.w;
        info.height = size.h;
        info.attachments = attachments;
        info.attachment_count = COUNT_OF(attachments);

        result_e result = xgl_create_framebuffer(&info, &framebuffer);
        check_result(result, "could not create framebuffer");
    }

    surface->framebuffer = framebuffer;

    ////////////////////////////////////////

    return RC_SUCCESS;

error:

    _destroy_buffers(surface);

    return RC_FAILURE;
}

struct screen_surface* screen_surface_create(struct screen_surface_create_info *ci)
{
    check_ptr(ci);

    check_expr(ci->type != SCREEN_SURFACE_TYPE_UNKNOWN);

    check_expr(ci->viewport.width > 0);
    check_expr(ci->viewport.height > 0);
    check_expr(ci->viewport.min_depth != ci->viewport.max_depth);

    ////////////////////////////////////////

    struct screen_surface *surface = calloc(1, sizeof(struct screen_surface));
    check_mem(surface);

    surface->type = ci->type;
    surface->viewport = ci->viewport;
    surface->clear_values = ci->clear_values;

    result_e result = _create_buffers(surface);
    check_result(result, "could not create buffers");

    ////////////////////////////////////////

    return surface;

error:
    if (surface) {
        screen_surface_destroy(surface);
    }

    return NULL;
}

void screen_surface_destroy(struct screen_surface *surface)
{
    check_ptr(surface);

    _destroy_buffers(surface);

    if (surface->type == SCREEN_SURFACE_TYPE_CPU)
    {
        surface->pb = (struct pixelbuffer) {0};
    }

    free(surface);

error:
    return;
}

bool screen_surface_begin(struct screen_surface* surface)
{
    struct xgl_render_pass_info pass_info = {0};
    pass_info.name = "Screen Surface Pass"; // make_string()
    pass_info.clear_values = surface->clear_values;
    pass_info.framebuffer = surface->framebuffer;

    xgl_set_viewports(1, &surface->viewport);

    bool render_pass_valid = xgl_begin_render_pass(&pass_info);

    if (surface->type == SCREEN_SURFACE_TYPE_CPU && render_pass_valid)
    {
        surface->pb.pixels = xgl_map_texture(surface->color_buffer);
        check_ptr(surface->pb.pixels);

        surface->pb.user_data = xgl_map_texture(surface->depth_stencil_buffer);
        check_ptr(surface->pb.user_data);
    }

    return render_pass_valid;

error:
    return false;
}

void screen_surface_end(struct screen_surface* surface)
{
    check_ptr(surface);

    xgl_end_render_pass();

    if (surface->type == SCREEN_SURFACE_TYPE_CPU)
    {
        surface->pb.pixels = NULL;
        xgl_unmap_texture(surface->color_buffer);

        surface->pb.user_data = NULL;
        xgl_unmap_texture(surface->depth_stencil_buffer);
    }

error:
    return;
}

struct vec2 screen_surface_get_size(struct screen_surface *surface)
{
    check_ptr(surface);

    return make_vec2(surface->viewport.width, surface->viewport.height);

error:
    return make_vec2_zero();
}

void screen_surface_set_size(struct screen_surface *surface, struct vec2 size)
{
    check_ptr(surface);

    check_expr(size.w > 0);
    check_expr(size.h > 0);

    surface->viewport.width = size.w;
    surface->viewport.height = size.h;

    _destroy_buffers(surface);
    _create_buffers(surface);

error:
    return;
}

enum screen_surface_type screen_surface_get_type(struct screen_surface *surface)
{
    check_ptr(surface);

    return surface->type;

error:
    return SCREEN_SURFACE_TYPE_UNKNOWN;
}

struct xgl_viewport screen_surface_get_viewport(struct screen_surface *surface)
{
    check_ptr(surface);

    return surface->viewport;

error:
    return (struct xgl_viewport) {0};
}

xgl_texture screen_surface_get_texture(struct screen_surface *surface)
{
    check_ptr(surface);

    return surface->color_buffer;

error:
    return (xgl_texture) {0};
}

struct pixelbuffer* screen_surface_get_pixelbuffer(struct screen_surface *surface)
{
    check_ptr(surface);

    check_expr(surface->type == SCREEN_SURFACE_TYPE_CPU);
    // FIXME check_expr( PIXELBUFFER_VALID );

    check_quiet(surface->pb.pixels != NULL); // FIXME

    return &surface->pb;

error:
    return NULL;
}
