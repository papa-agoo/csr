////////////////////////////////////////////////////////////////////////////////////////////////////

#include "model_viewer.h"

#include "scene_priv.h"
#include "renderer_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// input callbacks
extern void mv_on_kbd_key_press(struct keyboard_event *e);
extern void mv_on_mouse_move(struct mouse_event *e);
extern void mv_on_mouse_wheel_spin(struct mouse_event *e);

// forward declarations
static result_e _create_renderer();
static result_e _create_scene();

static struct camera_ctl* _camera_ctl_none_ptr();
static struct camera_ctl* _camera_ctl_fps_ptr();
static struct camera_ctl* _camera_ctl_orbital_ptr();

////////////////////////////////////////////////////////////

struct model_viewer
{
    bool is_initialized;

    struct scene scene;
    struct renderer renderer;

    struct model_viewer_conf conf;
};

static struct model_viewer g_mv = {0};

////////////////////////////////////////////////////////////

#define mv_ptr() (&g_mv)

#define mv_conf_ptr() (&mv_ptr()->conf)
#define mv_scene_ptr() (&mv_ptr()->scene)
#define mv_renderer_ptr() (&mv_ptr()->renderer)

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
        struct renderer_conf *renderer = &conf->renderer;
        {
            renderer_conf_defaults(renderer);

            // config override
            renderer->enable_rcpu = false;
        }

        // scene
        struct scene_conf *scene = &conf->scene;
        {
            scene_conf_defaults(scene);

            // config override
            // ...
        }
    }

    check_result(_create_renderer());
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
    renderer_quit(mv_renderer_ptr());
}

void model_viewer_tick()
{
    // process scene
    {
        struct shader_data_frame *frame_data = &mv_renderer_ptr()->shader_data.frame.buffer.cpu;

        struct camera *camera = mv_scene_ptr()->camera;
        struct camera_ctl *camera_ctl = mv_scene_ptr()->camera_ctl;

        // update camera
        {
            if (camera_ctl->update_cb) {
                camera_ctl->update_cb(camera, camera_ctl, aio_time_elapsed_delta());
            }

            f32 aspect_ratio = screen_get_aspect_ratio(mv_renderer_ptr()->screen.rgpu);

            frame_data->mat_view = camera_get_view_matrix(camera);
            frame_data->mat_projection = camera_get_persp_projection_matrix(camera, aspect_ratio);
            frame_data->mat_projection_ortho = camera_get_ortho_projection_matrix(camera, aspect_ratio);
        }

        // update model
        {
            // update model transform (model_ctl)
            // ...

            // update mesh_node hierarchy (compute mesh matrices)
            // ...
        }

        // FIXME streamline when model / mesh building is properly impl.
        {
            // temp debug primitives
            struct vec3 origin = {0};
            {
                if (camera_ctl->type == CAMERA_CTL_ORBITAL)
                {
                    struct camera_ctl_orbital *data = camera_ctl->data;
                    struct orbit *orbit = &data->orbit_src;

                    renderer_add_point(mv_renderer_ptr(), orbit->origin, make_vec3(1, 1, 1), 3, 0, false);

                    origin = orbit->origin;
                }

                struct mat44 m = mat44_translate(make_vec3(0, 1, 0));
                renderer_add_axes(mv_renderer_ptr(), m, false);
                renderer_add_aabb(mv_renderer_ptr(), m, make_aabb_unit_cube(), false);
            }

            // update gizmo shader data
            {
                struct mesh_gizmo *axes = &mv_renderer_ptr()->gizmo.axes;
                axes->data.mat_mvp = mat44_mult(mat44_mult(frame_data->mat_projection_ortho, frame_data->mat_view), mat44_translate(origin));
                axes->data.use_object_mvp = true;

                struct mesh_gizmo *grid = &mv_renderer_ptr()->gizmo.grid;
                grid->data = mv_renderer_ptr()->shader_data.object.buffer.cpu;
            }
        }
    }

    // draw frame
    renderer_tick(mv_renderer_ptr());
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
    clog_warn("not impl. yet");

    return RC_FAILURE;
}

void model_viewer_unload_model()
{
    clog_warn("not impl. yet");
}

struct renderer* model_viewer_get_renderer()
{
    return &mv_ptr()->renderer;
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
static result_e _create_renderer()
{
    // init renderer
    struct renderer *renderer = mv_renderer_ptr();
    renderer->conf = *mv_renderer_conf_ptr(); // FIXME

    check_result(renderer_init(renderer));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
