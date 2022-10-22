////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/graphics/xgl/driver.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum screen_usage_hint
{
    SCREEN_USAGE_HINT_UNKNOWN,

    SCREEN_USAGE_HINT_XGL_API,
    SCREEN_USAGE_HINT_NATIVE_API,
    SCREEN_USAGE_HINT_DIRECT_PIXEL_ACCESS,
};

const char* screen_usage_hint_cstr(enum screen_usage_hint hint);
const char* screen_usage_hint_cstr_human(enum screen_usage_hint hint);

enum screen_scaling_policy
{
    SCREEN_SCALING_POLICY_UNKNOWN, // guess (pixel access = static, else dynamic)

    SCREEN_SCALING_POLICY_STATIC,
    SCREEN_SCALING_POLICY_DYNAMIC,

    SCREEN_SCALE_POLICY_STRETCH,
};

enum screen_resize_policy
{
    SCREEN_RESIZE_POLICY_RESIZE_FRAMEBUFFER,
    SCREEN_RESIZE_POLICY_SCALE_VIEWPORT,
};

// resize policy
//      - resize fb
//      - scale vp
//      - keep aspect ratio

// scale policy
//      - integer
//      - floating point

////////////////////////////////////////////////////////////////////////////////

struct screen;

struct screen_create_info
{
    const char* name;

    u32 width;
    u32 height;
    f32 scale;

    struct vec4 clear_color;

    enum screen_usage_hint usage_hint;
    enum screen_scaling_policy scaling_policy;
};

struct screen* screen_create(struct screen_create_info *ci);
void screen_destroy(struct screen* screen);

bool screen_begin(struct screen* screen);
void screen_end();

const char* screen_get_name(struct screen* screen);
enum screen_usage_hint screen_get_usage_hint(struct screen* screen);

xgl_texture screen_get_texture(struct screen* screen);

void screen_resize(struct screen *screen, u32 width, u32 height);

// unscaled screen size (texture dimensions)
struct rect screen_get_rect(struct screen *screen);

// scaled screen size (viewport dimensions)
struct rect screen_get_scaled_rect(struct screen *screen);

// scaling api
void screen_scale_up(struct screen* screen);
void screen_scale_down(struct screen* screen);

f32 screen_get_scale(struct screen* screen);
void screen_set_scale(struct screen* screen, f32 factor);

f32 screen_get_max_scale(struct screen* screen);

void screen_maximize_scale(struct screen* screen);
void screen_reset_scale(struct screen* screen);

bool screen_is_scale_maxed(struct screen* screen);
