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

    // FIXME obsolete when the resource / asset system is impl.
    struct arena *arena;
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

    ////////////////////////////////////////

    // check_result(_create_resource_srv());
    {
        mv_ptr()->arena = arena_create(make_string("Model Viewer Assets"), MegaBytes(32));
        check_ptr(mv_ptr()->arena);
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

    model_viewer_set_model(NULL);

    scene_quit(mv_scene_ptr());
    rsx_quit();

    // resource_srv_quit();
    arena_destroy(mv_ptr()->arena);
}

void model_viewer_tick()
{
    csr_assert(mv_ptr()->is_initialized);

    f64 dt = aio_time_elapsed_delta();

    // process scene
    scene_tick(mv_scene_ptr(), dt);

    // prepare render data
    struct rsx_render_data *render_data = rsx_get_render_data();
    {
        struct camera *camera = mv_scene_ptr()->camera;

        // update global shader data
        struct shader_data_frame *frame_data = &render_data->frame.data.cpu;
        {
            f32 aspect_ratio = screen_get_aspect_ratio(rsx_ptr()->screen.rgpu);

            frame_data->mtx_view = camera_get_view_matrix(camera);
            frame_data->mtx_projection = camera_get_persp_projection_matrix(camera, aspect_ratio);
            frame_data->mtx_projection_ortho = camera_get_ortho_projection_matrix(camera, aspect_ratio);
        }

        // misc
        render_data->world_origin = scene_get_origin(mv_scene_ptr());
    }

    // FIXME gets the job done for now, obsolete when the scene system is in place
    struct rsx_pass_meshes *pass_meshes = rsx_pass_data_meshes_ptr();
    {
        vector_clear(pass_meshes->priv.meshes);

        struct model *model = scene_get_model(mv_scene_ptr());

        if (pass_meshes->enabled && model)
        {
            struct mat44 model_matrix = model_get_transform_matrix(model);

            struct rsx_mesh *mesh = model_get_mesh(model);

            if (mesh)
            {
                mesh->shader_data.data.cpu.mtx_model = model_matrix;

                vector_push_back(pass_meshes->priv.meshes, mesh);
            }
            else
            {
                // draw a red box as an indicator for an invalid / missing mesh
                rsx_debug_add_colored_aabb(model_matrix, make_aabb_unit_cube(), make_vec3(1, 0, 0), true);
            }
        }
    }

    // draw frame
    rsx_tick(dt);
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

struct model* model_viewer_get_model()
{
    return scene_get_model(mv_scene_ptr());
}

result_e model_viewer_set_model(struct model *model)
{
    struct model *model_old = scene_get_model(mv_scene_ptr());

    if (model_old) {
        model_destroy(model_old);
    }

    scene_set_model(mv_scene_ptr(), model);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

struct arena* model_viewer_get_resource_arena()
{
    return mv_ptr()->arena;
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
        init_info.arena = aio_get_main_arena();
        init_info.screen_rgpu = screen_rgpu;
        init_info.screen_rcpu = screen_rcpu;

        return rsx_init(&init_info);
    }

error:
    return RC_FAILURE;
}
