////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/screen_priv.h>

#include <csr/kernel/kio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// impl. notes on resize / scale policies
//
//  - screen CPU fb (pixelbuffer)
//      - always "fixed size" framebuffer + use upscaling
//
//  - screen GPU fb (framebuffer)
//      - screen size params available
//          - treat screen as "fixed size" framebuffer + use upscaling
//      - no screen size params
//          - use 75% of window size, framebuffer resizeable (no upscaling)

// - resize policy "AUTO"
//
//      - keep aspect ratio ON or OFF
//      - scale factor always 1 (no scaling, framebuffer resize)
//
//      - resize_screen_to_window_size()
//          - window_resize_event triggered (manual resize, docking)
//
//      - resize_window_to_screen_size()
//          - no window_resize_event triggered (screen resized externally)
//          - window size != screen size

// - resize policy "EXPLICIT"
//
//      - center_screen_within_window()
//          - keep aspect ratio ON
//          - scaled image (no framebuffer resize)
//          - centered on both axes (x,y)
//          - window_resize_event triggered (update scale factor to fit the new area)

////////////////////////////////////////////////////////////////////////////////

static f32 _calc_max_scale_factor(enum screen_scale_policy scale_policy, struct vec2 src, struct vec2 dst)
{
    // FIXME use scale policy

    f32 max_scale_w = floorf(dst.w / src.w);
    f32 max_scale_h = floorf(dst.h / src.h);

    return max(max_scale_w, max_scale_h);
}

////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////////////////////////
// resize api
////////////////////////////////////////////////////////////////////////////////////////////////////
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

    check_expr(factor >= 1.0);
    check_expr(factor <= screen_get_max_scale(screen));

    // FIXME use scale policy info

    screen->scale_factor = factor;

error:
    return;
}

void screen_scale_up(struct screen* screen)
{
    check_ptr(screen);

    // FIXME use scale policy info

    screen->scale_factor++;

    screen->scale_factor = min(screen->scale_factor, screen_get_max_scale(screen));

error:
    return;
}

void screen_scale_down(struct screen* screen)
{
    check_ptr(screen);

    // FIXME use scale policy info

    screen->scale_factor--;

    screen->scale_factor = max(screen->scale_factor, 1);

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

    screen->scale_factor = 1;

error:
    return;
}

f32 screen_get_max_scale(struct screen* screen)
{
    check_ptr(screen);
    check_ptr(screen->surface);

    struct vec2 max_size = kio_video_get_window_resolution();
    struct vec2 surface_size = screen_surface_get_size(screen->surface);

    return _calc_max_scale_factor(screen->scale_policy, surface_size, max_size);

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
