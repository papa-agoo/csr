////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "screen_surface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define SCREEN_SCALE_MIN        1
#define SCREEN_WIDTH_MIN        320
#define SCREEN_HEIGHT_MIN       160

// how to resize the screen (surface) size if the parent window size changes
enum screen_resize_policy
{
    SCREEN_RESIZE_POLICY_UNKNOWN,

    // adjust screen size automatically to match the parent window size
    SCREEN_RESIZE_POLICY_AUTO,

    // parent window resize has no effect on screen size
    SCREEN_RESIZE_POLICY_EXPLICIT,
};

// how to scale the screen contents to a bigger content area (parent window)
enum screen_scale_policy
{
    SCREEN_SCALE_POLICY_UNKNOWN,

    // no scaling wanted (factor always 1)
    SCREEN_SCALE_POLICY_NONE,

    // floating point
    SCREEN_SCALE_POLICY_FP,

    // integer (1, 2, ...)
    SCREEN_SCALE_POLICY_INTEGER,
};

////////////////////////////////////////////////////////////////////////////////

struct screen;

struct screen_stats
{
    u32 num_frames;
    f32 avg_fps;
    f32 avg_frametime_ms;
};

struct screen_create_info
{
    const char* name;

    f32 scale_factor;
    enum screen_scale_policy scale_policy;

    bool keep_aspect_ratio;
    enum screen_resize_policy resize_policy;

    struct screen_surface_create_info surface;
};

struct screen* screen_create(struct screen_create_info *ci);
void screen_destroy(struct screen* screen);

bool screen_begin(struct screen* screen, enum screen_surface_type surface_type);
void screen_end();

const char* screen_get_name(struct screen* screen);

const char* screen_get_surface_type_cstr(struct screen* screen);
enum screen_surface_type screen_get_surface_type(struct screen* screen);

struct xgl_viewport screen_get_viewport(struct screen* screen);
xgl_texture screen_get_texture(struct screen* screen);
struct pixelbuffer* screen_get_pixelbuffer(struct screen* screen);

const struct screen_stats* screen_get_stats(struct screen *screen);

bool screen_get_keep_aspect_ratio(struct screen* screen);
void screen_set_keep_aspect_ratio(struct screen* screen, bool keep);
void screen_toggle_keep_aspect_ratio(struct screen* screen);

// resize api
enum screen_resize_policy screen_get_resize_policy(struct screen* screen);
void screen_set_resize_policy(struct screen* screen, enum screen_resize_policy policy);

struct vec2 screen_get_min_size(struct screen *screen);
struct vec2 screen_get_max_size(struct screen *screen);

struct vec2 screen_get_size(struct screen *screen);
void screen_set_size(struct screen *screen, struct vec2 size);

struct vec2 screen_get_scaled_size(struct screen *screen);
struct vec2 screen_get_size_for_parent(struct screen *screen, struct vec2 parent);


// scaling api
enum screen_scale_policy screen_get_scale_policy(struct screen *screen);
void screen_set_scale_policy(struct screen *screen, enum screen_scale_policy policy);

f32 screen_get_scale(struct screen* screen);
void screen_set_scale(struct screen* screen, f32 factor);

void screen_scale_up(struct screen* screen);
void screen_scale_down(struct screen* screen);

void screen_maximize_scale(struct screen* screen);
void screen_reset_scale(struct screen* screen);

f32 screen_get_max_scale(struct screen* screen);
f32 screen_get_max_scale_for_parent(struct screen *screen, struct vec2 parent);

bool screen_is_scale_maxed(struct screen* screen);
