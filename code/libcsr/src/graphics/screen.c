////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/screen_priv.h>

#include <csr/kernel/kio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct screen *g_active_screen = NULL;

struct screen* screen_create(struct screen_create_info *ci)
{
    check_ptr(ci);
    check_ptr(ci->name);

    check_expr(ci->scale_factor >= SCREEN_SCALE_MIN);

    check_expr(ci->scale_policy != SCREEN_SCALE_POLICY_UNKNOWN);
    check_expr(ci->resize_policy != SCREEN_RESIZE_POLICY_UNKNOWN);

    check_expr(ci->surface.type != SCREEN_SURFACE_TYPE_UNKNOWN);

    check_expr(ci->surface.viewport.width >= SCREEN_WIDTH_MIN);
    check_expr(ci->surface.viewport.height >= SCREEN_HEIGHT_MIN);
    check_expr(ci->surface.viewport.min_depth != ci->surface.viewport.max_depth);

    // alpha channel must be 1, else its treaten as undefined
    check_expr(ci->surface.clear_values.color.a == 1.0);

    // FIXME depth / stencil values?

    ////////////////////////////////////////

    struct screen *screen = calloc(1, sizeof(struct screen));
    check_mem(screen);

    screen->name = strdup(ci->name);
    screen->scale_factor = ci->scale_factor;
    screen->scale_policy = ci->scale_policy;
    screen->keep_aspect_ratio = ci->keep_aspect_ratio;
    screen->resize_policy = ci->resize_policy;

    // create surface
    screen->surface = screen_surface_create(&ci->surface);
    check_ptr(screen->surface);

    ////////////////////////////////////////

    return screen;

error:
    if (screen) {
        screen_destroy(screen);
    }

    return NULL;
}

void screen_destroy(struct screen* screen)
{
    check_ptr(screen);

    if (screen->surface) {
        screen_surface_destroy(screen->surface);
    }

    free(screen);

error:
    return;   
}

bool screen_begin(struct screen* screen, enum screen_surface_type surface_type)
{
    check_ptr(screen);

    // only one screen at a time may be used
    check_expr(g_active_screen == NULL);

    // weak surface type validation
    check_expr(screen_surface_get_type(screen->surface) == surface_type);

    if(screen_surface_begin(screen->surface))
    {
        // mark this screen as being used (active render pass)
        g_active_screen = screen;

        return true;
    }

error:
    return false;
}

void screen_end()
{
    check_expr(g_active_screen != NULL);

    screen_surface_end(g_active_screen->surface);

    // this screen is not used anymore
    g_active_screen = NULL;

error:
    return;   
}

const char* screen_get_name(struct screen* screen)
{
    check_ptr(screen);

    return screen->name;

error:
    return NULL;
}

enum screen_surface_type screen_get_surface_type(struct screen* screen)
{
    check_ptr(screen);
    check_ptr(screen->surface);

    return screen_surface_get_type(screen->surface);

error:
    return SCREEN_SURFACE_TYPE_UNKNOWN;
}

xgl_texture screen_get_texture(struct screen* screen)
{
    check_ptr(screen);

    return screen_surface_get_texture(screen->surface);

error:
    return (xgl_texture) {0};
}

struct pixelbuffer* screen_get_pixelbuffer(struct screen* screen)
{
    check_ptr(screen);

    check_expr(g_active_screen != NULL);

    return screen_surface_get_pixelbuffer(screen->surface);

error:
    return NULL;
}

bool screen_get_keep_aspect_ratio(struct screen* screen)
{
    check_ptr(screen);

    return screen->keep_aspect_ratio;

error:
    return true;
}

void screen_set_keep_aspect_ratio(struct screen* screen, bool keep)
{
    check_ptr(screen);

    screen->keep_aspect_ratio = keep;

error:
    return;
}

void screen_toggle_keep_aspect_ratio(struct screen* screen)
{
    check_ptr(screen);

    screen->keep_aspect_ratio ^= 1;

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// resize api
////////////////////////////////////////////////////////////////////////////////////////////////////
static struct vec2 _calc_scale_factor(enum screen_scale_policy scale_policy, struct vec2 src, struct vec2 dst)
{
    // FIXME aspect ratio for both axes?
    f32 ratio = min(dst.w / src.w, dst.h / src.h);

    switch (scale_policy)
    {
        case SCREEN_SCALE_POLICY_FP:
            return make_vec2(ratio, ratio);

        case SCREEN_SCALE_POLICY_INTEGER:
            return make_vec2(.min = floorf(ratio), .max = ceilf(ratio));
    }

    // SCREEN_SCALE_POLICY_NONE
    return make_vec2_one();
}

enum screen_resize_policy screen_get_resize_policy(struct screen* screen)
{
    check_ptr(screen);

    return screen->resize_policy;

error:
    return SCREEN_RESIZE_POLICY_UNKNOWN;
}

void screen_set_resize_policy(struct screen* screen, enum screen_resize_policy policy)
{
    check_ptr(screen);
    check_expr(policy != SCREEN_RESIZE_POLICY_UNKNOWN);

    screen->resize_policy = policy;

error:
    return;
}

struct vec2 screen_get_min_size(struct screen *screen)
{
    check_ptr(screen);

    return make_vec2(SCREEN_WIDTH_MIN, SCREEN_HEIGHT_MIN);

error:
    return make_vec2_zero();
}

struct vec2 screen_get_max_size(struct screen *screen)
{
    check_ptr(screen);

    return kio_video_get_window_resolution();

error:
    return make_vec2_zero();
}

struct vec2 screen_get_size(struct screen *screen)
{
    check_ptr(screen);
    check_ptr(screen->surface);

    return screen_surface_get_size(screen->surface);

error:
    make_vec2_zero();
}

void screen_set_size(struct screen *screen, struct vec2 size)
{
    check_ptr(screen);
    check_ptr(screen->surface);

    check_expr(size.w >= SCREEN_WIDTH_MIN && size.h >= SCREEN_HEIGHT_MIN);

    struct vec2 max_size = screen_get_max_size(screen);
    check_expr(size.w <= max_size.w && size.h <= max_size.h);

    screen_surface_set_size(screen->surface, size);

error:
    return;
}

struct vec2 screen_get_scaled_size(struct screen *screen)
{
    check_ptr(screen);

    return vec2_scale(screen_get_size(screen), screen->scale_factor);

error:
    return make_vec2_zero();
}

struct vec2 screen_get_size_for_parent(struct screen *screen, struct vec2 parent)
{
    check_ptr(screen);
    check_expr(parent.x > 0 && parent.y > 0);

    if (screen->keep_aspect_ratio)
    {
        struct vec2 child = screen_get_size(screen);
        struct vec2 scale = _calc_scale_factor(screen->scale_policy, child, parent);

        return vec2_scale(child, scale.max);
    }

error:
    return parent;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// scaling api
////////////////////////////////////////////////////////////////////////////////////////////////////
enum screen_scale_policy screen_get_scale_policy(struct screen *screen)
{
    check_ptr(screen);

    return screen->scale_policy;

error:
    return SCREEN_SCALE_POLICY_UNKNOWN;
}

void screen_set_scale_policy(struct screen *screen, enum screen_scale_policy policy)
{
    check_ptr(screen);
    check_expr(policy != SCREEN_SCALE_POLICY_UNKNOWN);

    screen->scale_policy = policy;

error:
    return;
}

f32 screen_get_scale(struct screen* screen)
{
    check_ptr(screen);

    return screen->scale_factor;

error:
    return 0;
}

void screen_set_scale(struct screen* screen, f32 factor)
{
    check_ptr(screen);

    if (screen->scale_policy == SCREEN_SCALE_POLICY_NONE) {
        return;
    }

    check_expr(factor >= SCREEN_SCALE_MIN);
    check_expr(factor <= screen_get_max_scale(screen));

    if (screen->scale_policy == SCREEN_SCALE_POLICY_INTEGER) {
        factor = floorf(factor);
    }

    screen->scale_factor = factor;

error:
    return;
}

void screen_scale_up(struct screen* screen)
{
    check_ptr(screen);

    if (screen->scale_policy == SCREEN_SCALE_POLICY_INTEGER)
    {
        screen->scale_factor++;

        screen->scale_factor = min(screen->scale_factor, screen_get_max_scale(screen));
    }

error:
    return;
}

void screen_scale_down(struct screen* screen)
{
    check_ptr(screen);

    if (screen->scale_policy == SCREEN_SCALE_POLICY_INTEGER)
    {
        screen->scale_factor--;

        screen->scale_factor = max(screen->scale_factor, SCREEN_SCALE_MIN);
    }

error:
    return;
}

void screen_maximize_scale(struct screen* screen)
{
    check_ptr(screen);

    screen->scale_factor = screen_get_max_scale(screen);

error:
    return;
}

void screen_reset_scale(struct screen* screen)
{
    check_ptr(screen);

    screen->scale_factor = SCREEN_SCALE_MIN;

error:
    return;
}

f32 screen_get_max_scale(struct screen* screen)
{
    check_ptr(screen);

    struct vec2 parent = screen_get_max_size(screen);

    return screen_get_max_scale_for_parent(screen, parent);

error:
    return 0;
}

f32 screen_get_max_scale_for_parent(struct screen *screen, struct vec2 parent)
{
    check_ptr(screen);
    check_expr(parent.x > 0 && parent.y > 0);

    // scale factor for child / parent
    struct vec2 child = screen_get_size(screen);
    struct vec2 scale = _calc_scale_factor(screen->scale_policy, child, parent);

    // scale factor for child / parent_fb (max possible parent (framebuffer) size)
    struct vec2 parent_fb = screen_get_max_size(screen);
    struct vec2 scale_fb = _calc_scale_factor(screen->scale_policy, child, parent_fb);

    return clamp(scale.min, SCREEN_SCALE_MIN, scale_fb.max);

error:
    return 0;
}

bool screen_is_scale_maxed(struct screen* screen)
{
    check_ptr(screen);

    return (screen->scale_factor == screen_get_max_scale(screen));

error:
    return false;
}
