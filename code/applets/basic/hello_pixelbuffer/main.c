////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/applet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void draw_shadertoy_bg(struct pixelbuffer* pb, f64 time_elapsed)
{
    check_ptr(pb);

    // ported from https://www.shadertoy.com/new
    struct vec2 uv = {0};
    struct vec4 color = {.a = 1.0};
    struct vec2 canvas = {pb->width, pb->height};

    for (u32 y = 0; y < canvas.h; y++)
    {
        uv.y = y / canvas.h;

        color.g = 0.5f + 0.5f * cosf(time_elapsed + uv.y + 2.0f);

        for (u32 x = 0; x < canvas.w; x++)
        {
            uv.x = x / canvas.w;

            color.r = 0.5f + 0.5f * cosf(time_elapsed + uv.x + 0.0f);
            color.b = 0.5f + 0.5f * cosf(time_elapsed + uv.x + 4.0f);

            // safe write access
            // pixelbuffer_draw_point(pb, x, y, make_color32_from_vec4(color));

            // fast write access
            pixelbuffer_draw_point_fast(pb, x, y, make_color32_from_vec4(color));
        }
    }

error:
    return;
}

static void draw_line_circle(struct pixelbuffer *pb)
{
    check_ptr(pb);

    struct vec2 p1 = make_vec2(pb->width / 2.0, pb->height / 2.0);
    struct vec2 p2 = {0};

    u32 steps = 8;
    f32 padding = 20;
    f32 radius = p1.y - padding;

    for (u32 i = 0; i < 360; i+=steps)
    {
        p2.x = cos(i * M_PI / 180.0) * radius;
        p2.y = sin(i * M_PI / 180.0) * radius;

        p2.x = p1.x + p2.x;
        p2.y = p1.y - p2.y;

        pixelbuffer_draw_line(pb, p1, p2, make_color32(COLOR_WHITE));
    }

error:
    return;
}

static struct string _plugin_get_name()
{
    return make_string("Hello Pixelbuffer");
}

static struct string _plugin_get_description()
{
    return make_string("Pixelbuffer rasterization Examples.");
}

static struct version _plugin_get_version()
{
    return make_version(1, 0, 0);
}

static result_e _plugin_startup()
{
    // create screen
    struct screen_create_info create_info = {0};
    create_info.name = make_string("Pixelbuffer");
    create_info.scale_policy = SCREEN_SCALE_POLICY_FP;

    create_info.surface.type = SCREEN_SURFACE_TYPE_CPU;
    create_info.surface.viewport.width = 640;
    create_info.surface.viewport.height = 360;
    create_info.surface.clear_values.color = make_vec4(0, 0, 1, 1);

    struct screen *screen = aio_add_screen("pb", &create_info);
    check_ptr(screen);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _plugin_tick()
{
    struct screen *screen = aio_get_screen("pb");

    if (screen_begin(screen, SCREEN_SURFACE_TYPE_CPU))
    {
        struct pixelbuffer *pb = screen_get_pixelbuffer(screen);

        if (pb)
        {
            // draw background
            draw_shadertoy_bg(pb, aio_time_elapsed());

            // draw lines
            draw_line_circle(pb);

            // fill some rects
            u32 x = 10, y = 10, w = 20, h = 20;

            pixelbuffer_fill_rect(pb, make_rect(x, y, w, h), make_color32(COLOR_RED));
            pixelbuffer_fill_rect(pb, make_rect(x + w, y, w, h), make_color32(COLOR_GREEN));
            pixelbuffer_fill_rect(pb, make_rect(x + (w * 2), y, w, h), make_color32(COLOR_BLUE));
        }

        screen_end();
    }
}

////////////////////////////////////////////////////////////////////////////////

void applet_plugin_export(struct applet_plugin* plugin)
{
    plugin->get_name = _plugin_get_name;
    plugin->get_description = _plugin_get_description;
    plugin->get_version = _plugin_get_version;

    plugin->startup = _plugin_startup;
    plugin->tick = _plugin_tick;
}
