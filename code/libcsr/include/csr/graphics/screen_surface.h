////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/graphics/xgl/driver.h>
#include <csr/graphics/pixelbuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum screen_surface_type
{
    SCREEN_SURFACE_TYPE_UNKNOWN,

    SCREEN_SURFACE_TYPE_GPU,
    SCREEN_SURFACE_TYPE_CPU,
};

CSR_INLINE const char* screen_surface_type_cstr(enum screen_surface_type type)
{
    switch (type)
    {
        case SCREEN_SURFACE_TYPE_GPU:
            return "GPU Surface";

        case SCREEN_SURFACE_TYPE_CPU:
            return "CPU Surface";

        default:
            return "Unknown";
    }
}

////////////////////////////////////////////////////////////////////////////////

struct screen_surface;

struct screen_surface_create_info
{
    enum screen_surface_type type;

    struct xgl_viewport viewport;
    struct xgl_clear_values clear_values;
};

struct screen_surface* screen_surface_create(struct screen_surface_create_info *ci);
void screen_surface_destroy(struct screen_surface *surface);

bool screen_surface_begin(struct screen_surface* surface);
void screen_surface_end(struct screen_surface* surface);

struct vec2 screen_surface_get_size(struct screen_surface *surface);
void screen_surface_set_size(struct screen_surface *surface, struct vec2 size);

enum screen_surface_type screen_surface_get_type(struct screen_surface *surface);

xgl_texture screen_surface_get_texture(struct screen_surface *surface);
struct pixelbuffer* screen_surface_get_pixelbuffer(struct screen_surface *surface);
