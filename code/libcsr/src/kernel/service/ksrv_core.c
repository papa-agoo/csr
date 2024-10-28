////////////////////////////////////////////////////////////////////////////////////////////////////

#define KLOG_MODULE_NAME kernel

#include <csr/kernel.h>
#include <csr/kernel/kernel_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _on_error(s32 id, struct string message)
{
    struct event event = {0};

    event.type = EVENT_WINDOW_ERROR;
    event.window.error_id = id;
    event.window.error_msg = string_is_valid(message) ? message : make_string("???");

    event_bus_push_event(&event);
}

static void _on_window_close()
{
    struct event event = {0};

    event.type = EVENT_WINDOW_CLOSE;

    event_bus_push_event(&event);
}

static void _on_window_resize(s32 width, s32 height)
{
    struct event event = {0};

    event.type = EVENT_WINDOW_RESIZE;
    event.window.width = width;
    event.window.height = height;

    event_bus_push_event(&event);
}

static void _on_mouse_move(f64 x, f64 y)
{
    static f64 x_last = 0;
    static f64 y_last = 0;

    struct event event = {0};

    event.type = EVENT_MOUSE_MOVE;
    event.mouse.x = x;
    event.mouse.y = y;

    // relative mouse position to the last event
    event.mouse.x_delta = (x - x_last);
    event.mouse.y_delta = (y - y_last);

    // save current mouse position for the next event
    x_last = x;
    y_last = y;

    event_bus_push_event(&event);
}

static void _on_mouse_wheel_change(f64 x, f64 y)
{
    struct event event = {0};

    event.type = EVENT_MOUSE_WHEEL;
    event.mouse.wx = x;
    event.mouse.wy = y;

    event_bus_push_event(&event);
}

static void _on_mouse_button_change(enum mouse_button button, bool is_down)
{
    struct event event = {0};

    event.type = (is_down) ? EVENT_MOUSE_BUTTON_DOWN : EVENT_MOUSE_BUTTON_UP;
    event.mouse.button = button;

    event_bus_push_event(&event);
}

static void _on_keyboard_key_change(enum keyboard_key key, bool is_down)
{
    struct event event = {0};

    event.type = (is_down) ? EVENT_KBD_KEY_DOWN : EVENT_KBD_KEY_UP;
    event.keyboard.key = key;

    event_bus_push_event(&event);
}

////////////////////////////////////////////////////////////////////////////////

static void _update_global_clocks()
{
    struct ksrv_core *srv = ksrv_core_ptr();

    struct clock* clock = srv->clock;

    f64 current_time_s = platform_time_elapsed();
    f64 delta_s = current_time_s - clock_time_elapsed(clock);

    clock_tick(clock, delta_s);
}

static void _update_frame_stats()
{
    struct ksrv_core *srv = ksrv_core_ptr();

    struct frame_stats* frame = &srv->frame_stats;

    f64 time_elapsed = clock_time_elapsed(srv->clock);

    static f32 diff_runtime_s = 0.0f;
    static u32 diff_num_frames = 0;

    diff_runtime_s += time_elapsed - frame->runtime_s;
    diff_num_frames++;

    frame->runtime_s = time_elapsed;
    frame->runtime_ms = time_elapsed * 1000.0f;
    frame->num_frames++;

    f32 diff_timeframe_s = 0.50f;

    if (diff_runtime_s >= diff_timeframe_s)
    {
        frame->avg_fps = diff_num_frames / diff_runtime_s;
        frame->avg_frametime_ms = (diff_runtime_s * 1000.0f) / diff_num_frames;

        diff_runtime_s = 0.0f;
        diff_num_frames = 0;
    }
}

static void _kernel_tick_dummy() {}

static bool _mainloop_tick(struct mainloop* mainloop)
{
    platform_poll_events();
    event_bus_dispatch_events();

    // FIXME
    {
        ksrv_video_prepare_framebuffer();

        ksrv_core_ptr()->kernel_tick_cb();

        ksrv_video_present_framebuffer();
    }

    platform_swap_buffers();

    _update_global_clocks();
    _update_frame_stats();

    // reset arena for the next frame (no memset(0))
    arena_reset(ksrv_core_ptr()->allocator.arena_frame);

    return mainloop->is_running;
}

////////////////////////////////////////////////////////////////////////////////

result_e ksrv_core_init(struct ksrv_core_init_info *init_info)
{
    check_ptr(init_info);
    check_ptr(init_info->conf);
    check_ptr(init_info->platform_info);

    ////////////////////////////////////////

    struct ksrv_core *srv = ksrv_core_ptr();
    srv->conf = init_info->conf;

    ////////////////////////////////////////

    klog_info("initializing core service ...");

    result_e result = platform_init(init_info->platform_info);
    check_result(result, "could not init platform");

    // show platform info
    const struct platform_backend_info *backend = &platform_get_info()->backend;

    klog_info(" - os backend : %S v%d.%d.%d", &backend->name,
        backend->version.major, backend->version.minor, backend->version.micro);

    const struct platform_renderer_info *renderer = &platform_get_info()->renderer;

    klog_info(" - render api : %S v%d.%d.%d (GLSL v%S)", &renderer->name,
        renderer->version.major, renderer->version.minor, renderer->version.micro, &renderer->version_shading_lang);

    // set callbacks
    struct platform_callbacks *callbacks = platform_get_callbacks();
    callbacks->on_error = _on_error;
    callbacks->on_window_close = _on_window_close;
    callbacks->on_window_resize = _on_window_resize;
    callbacks->on_mouse_move = _on_mouse_move;
    callbacks->on_mouse_wheel_change = _on_mouse_wheel_change;
    callbacks->on_mouse_button_change = _on_mouse_button_change;
    callbacks->on_keyboard_key_change = _on_keyboard_key_change;

    ////////////////////////////////////////

    // create allocators
    srv->allocator.arena_main = make_arena();
    check_ptr(srv->allocator.arena_main);

    srv->allocator.arena_frame = make_arena();
    check_ptr(srv->allocator.arena_frame);

    // create clock
    srv->clock = clock_create(make_string("main"));
    check_ptr(srv->clock);

    // init mainloop
    srv->mainloop = mainloop_create();
    check_ptr(srv->mainloop);

    srv->mainloop->api->tick = _mainloop_tick;

    // set tick callback
    srv->kernel_tick_cb = init_info->kernel_tick_cb;

    if (!srv->kernel_tick_cb) {
        srv->kernel_tick_cb = _kernel_tick_dummy;
    }

    ////////////////////////////////////////

    srv->is_initialized = true;

    return RC_SUCCESS;

error:
    // FIXME cleaup
    return RC_FAILURE;
}

void ksrv_core_quit()
{
    klog_info("quitting core service ...");

    struct ksrv_core *srv = ksrv_core_ptr();

    ////////////////////////////////////////

    mainloop_destroy(srv->mainloop);

    clock_destroy(srv->clock);

    platform_quit();

    log_db_destroy(srv->log_db);

    env_vars_destroy(srv->env_vars);

    arena_destroy(srv->allocator.arena_frame);
    arena_destroy(srv->allocator.arena_main);

    ////////////////////////////////////////

    srv->is_initialized = false;
}

struct log_db* ksrv_get_log_db()
{
    struct ksrv_core *srv = ksrv_core_ptr();

    // logging is needed before kernel init, so ...
    if (!srv->log_db)
    {
        struct log_db_create_info log_db_info = {0};
        log_db_info.name = make_string("Kernel");
        log_db_info.max_messages = KSRV_CORE_MAX_LOG_MESSAGES;

        srv->log_db = log_db_create(&log_db_info);
        check_ptr(srv->log_db);
    }

    return srv->log_db;

error:
    return NULL;
}

struct env_vars* ksrv_get_env_vars()
{
    struct ksrv_core *srv = ksrv_core_ptr();

    if (!srv->env_vars)
    {
        srv->env_vars = env_vars_create();
        check_ptr(srv->env_vars);
    }

    return srv->env_vars;

error:
    return NULL;
}
