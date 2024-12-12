////////////////////////////////////////////////////////////////////////////////////////////////////

#include "model_viewer.h"

#include "scene_priv.h"
#include "rsx_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// input callbacks
extern void mv_on_kbd_key_press(struct keyboard_event *e);
extern void mv_on_mouse_move(struct mouse_event *e);
extern void mv_on_mouse_wheel_spin(struct mouse_event *e);

// forward declarations
static result_e _create_rsx();
static result_e _create_scene();

static struct camera_ctl* _camera_ctl_none_ptr();
static struct camera_ctl* _camera_ctl_fps_ptr();
static struct camera_ctl* _camera_ctl_orbital_ptr();

////////////////////////////////////////////////////////////

struct model_viewer
{
    bool is_initialized;

    struct scene scene;

    struct model_viewer_conf conf;
};

static struct model_viewer g_mv = {0};

////////////////////////////////////////////////////////////

#define mv_ptr() (&g_mv)

#define mv_conf_ptr() (&mv_ptr()->conf)
#define mv_scene_ptr() (&mv_ptr()->scene)

////////////////////////////////////////////////////////////

result_e model_viewer_init()
{
    csr_assert(!mv_ptr()->is_initialized);

    ////////////////////////////////////////

    struct model_viewer_conf *conf = mv_conf_ptr();
    {
        // model viewer
        // ...

        // renderer
        struct rsx_conf *rsx = &conf->rsx;
        {
            rsx_conf_defaults(rsx);

            // config override
            rsx->enable_rcpu = false;
        }

        // scene
        struct scene_conf *scene = &conf->scene;
        {
            scene_conf_defaults(scene);

            // config override
            // ...
        }
    }

    check_result(_create_rsx());
    check_result(_create_scene());

    ////////////////////////////////////////

    struct hid_callbacks *hid_cbs = aio_get_hid_callbacks();
    {
        hid_cbs->on_mouse_move = mv_on_mouse_move;
        hid_cbs->on_mouse_wheel_spin = mv_on_mouse_wheel_spin;
        hid_cbs->on_kbd_key_down = mv_on_kbd_key_press;
    }

    ////////////////////////////////////////

    mv_ptr()->is_initialized = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void model_viewer_quit()
{
    csr_assert(mv_ptr()->is_initialized);

    scene_quit(mv_scene_ptr());
    rsx_quit();
}

void model_viewer_tick()
{
    csr_assert(mv_ptr()->is_initialized);

    // process scene
    // ...

    // draw frame
    rsx_tick();
}

struct model_viewer_conf* model_viewer_get_conf()
{
    return &mv_ptr()->conf;
}

struct scene* model_viewer_get_scene()
{
    return &mv_ptr()->scene;
}

void model_viewer_set_camera_controller(enum camera_ctl_type type)
{
    switch (type)
    {
        case CAMERA_CTL_ORBITAL:
            mv_scene_ptr()->camera_ctl = _camera_ctl_orbital_ptr();
        break;

        case CAMERA_CTL_FIRST_PERSON:
            mv_scene_ptr()->camera_ctl = _camera_ctl_fps_ptr();
        break;

        default:
            mv_scene_ptr()->camera_ctl = _camera_ctl_none_ptr();
    }
}

result_e model_viewer_load_model(struct string path)
{
    check_expr(string_is_valid(path));

    clog_notice("not impl. yet");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void model_viewer_unload_model()
{
    clog_notice("not impl. yet");

error:
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// scene
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _camera_ctl_orbital_update_cb(struct camera *camera, struct camera_ctl *ctl, f64 dt)
{
    check_ptr(camera);
    check_ptr(ctl);

    check_expr(ctl->type == CAMERA_CTL_ORBITAL);

    struct camera_ctl_orbital *data = ctl->data;
    check_ptr(data);

    orbit_set_azimuth(&data->orbit_dst, data->orbit_dst.azimuth + data->animate);

    if (data->interpolate)
    {
        struct scene_camera_conf *conf = &mv_scene_conf_ptr()->camera;
        f32 value = conf->orbital.interpolation_speed * dt;

        struct orbit *src = &data->orbit_src;
        struct orbit *dst = &data->orbit_dst;

        src->origin = vec3_lerp(src->origin, dst->origin, value);
        src->azimuth = lerp(value, src->azimuth, dst->azimuth);
        src->polar = lerp(value, src->polar, dst->polar);
        src->radius = lerp(value, src->radius, dst->radius);
    }
    else
    {
        data->orbit_src = data->orbit_dst;
    }

    struct orbit *orbit = &data->orbit_src;
    struct transform *transform = camera_get_transform(camera);

    transform_lookat_from(transform, orbit_get_coords(orbit), orbit->origin);

error:
    return;
}

static struct camera_ctl* _camera_ctl_orbital_ptr()
{
    static bool is_initialized = false;

    static struct camera_ctl_orbital data = {0};

    if (!is_initialized)
    {
        struct scene_camera_conf *conf = &mv_scene_conf_ptr()->camera;

        data.interpolate = conf->orbital.interpolate;
        data.orbit_dst.azimuth = conf->orbital.azimuth;
        data.orbit_dst.polar = conf->orbital.polar;
        data.orbit_dst.radius = conf->orbital.radius;
        data.orbit_src = data.orbit_dst;

        is_initialized = true;
    }

    static struct camera_ctl ctl = {
        .type = CAMERA_CTL_ORBITAL,
        .update_cb = _camera_ctl_orbital_update_cb,
        .data = &data,
    };

    return &ctl;
}

static void _camera_ctl_fps_update_cb(struct camera *camera, struct camera_ctl *ctl, f64 dt)
{
    // ...
}

static struct camera_ctl* _camera_ctl_fps_ptr()
{
    static struct camera_ctl ctl = {
        .type = CAMERA_CTL_FIRST_PERSON,
        .update_cb = _camera_ctl_fps_update_cb,
    };

    return &ctl;
}

static struct camera_ctl* _camera_ctl_none_ptr()
{
    static struct camera_ctl ctl = {
        .type = CAMERA_CTL_NONE
    };

    return &ctl;
}

static result_e _create_scene()
{
    struct scene_conf *conf = mv_scene_conf_ptr();

    // create minimal scene
    struct scene *scene = mv_scene_ptr();
    {
        struct camera_create_info camera_info = {0};
        camera_info.name = make_string("main");
        camera_info.mode = CAMERA_PROJECTION_PERSPECTIVE;

        struct scene_init_info init_info = {0};
        init_info.conf = conf;
        init_info.camera_info = &camera_info;

        check_result(scene_init(&init_info, scene));
    }

    // set default camera controller
    scene->camera_ctl = _camera_ctl_orbital_ptr();

    // set model
    // ...

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// renderer
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _create_rsx()
{
    struct rsx_conf *conf = mv_rsx_conf_ptr();

    struct xgl_clear_values clear_values = {0};
    clear_values.color = make_vec4_3_1(conf->color.background, 1.0);
    clear_values.depth = 1.0f;

    // create rgpu screen
    struct screen *screen_rgpu = NULL;
    {
        struct screen_create_info info = {0};
        info.name = make_string("GPU Renderer");
        info.is_suspended = !conf->enable_rgpu;

        info.surface.type = SCREEN_SURFACE_TYPE_GPU;
        info.surface.clear_values = clear_values;
        info.surface.viewport.width = 1280;
        info.surface.viewport.height = 720;

        screen_rgpu = aio_add_screen("rgpu", &info);
    }

    // create rcpu screen
    struct screen *screen_rcpu = NULL;
    {
        struct screen_create_info info = {0};
        info.name = make_string("CPU Renderer");
        info.is_suspended = !conf->enable_rcpu;

        info.surface.type = SCREEN_SURFACE_TYPE_CPU;
        info.surface.clear_values = clear_values;
        info.surface.viewport.width = 640;
        info.surface.viewport.height = 360;

        screen_rcpu = aio_add_screen("rcpu", &info);
    }

    // create renderer
    {
        struct rsx_init_info init_info = {0};
        init_info.conf = conf;
        init_info.screen_rgpu = screen_rgpu;
        init_info.screen_rcpu = screen_rcpu;

        return rsx_init(&init_info);
    }

error:
    return RC_FAILURE;
}
