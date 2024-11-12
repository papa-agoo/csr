////////////////////////////////////////////////////////////////////////////////////////////////////

#include "model_viewer.h"

#include "scene_priv.h"
#include "renderer_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct model_viewer
{
    bool is_initialized;

    struct scene scene;
    struct renderer renderer;

    struct mv_conf conf;
};

static struct model_viewer g_mv = {0};

#define mv_ptr() (&g_mv)
#define mv_conf_ptr() (&mv_ptr()->conf)

#define mv_scene_ptr() (&mv_ptr()->scene)
#define mv_scene_cache_ptr() (&mv_scene_ptr()->cache)

#define mv_renderer_ptr() (&mv_ptr()->renderer)

static void on_kbd_key_press(struct keyboard_event *e)
{
    struct camera_ctl *ctl = model_viewer_get_camera_ctl();

    if (ctl->type == CAMERA_CTL_ORBITAL)
    {
        struct camera_ctl_orbital *data = ctl->data;
        struct orbit *orbit = &data->orbit_dst;

        switch (e->key)
        {
            // reset view
            case KBD_KEY_KP_0:
                orbit_set(orbit, make_vec3_zero(), 15, 75, 100);
            return;

            // azimuth / polar controls
            case KBD_KEY_LEFT:
                orbit_set_azimuth(orbit, orbit->azimuth - 1);
            return;

            case KBD_KEY_RIGHT:
                orbit_set_azimuth(orbit, orbit->azimuth + 1);
            return;

            case KBD_KEY_UP:
                orbit_set_polar(orbit, orbit->polar - 1);
            return;

            case KBD_KEY_DOWN:
                orbit_set_polar(orbit, orbit->polar + 1);
            return;

            // toggle projection type (perspective / ortho)
            case KBD_KEY_KP_5:
                alog_warn("toggle projection type (persp/ortho) not impl. yet");
            return;

            // top view
            case KBD_KEY_PAGE_UP:
                orbit_set_orientation(orbit, ORBIT_ORIENTATION_TOP);
            return;

            // bottom view
            case KBD_KEY_PAGE_DOWN:
                orbit_set_orientation(orbit, ORBIT_ORIENTATION_BOTTOM);
            return;

            // rear view
            case KBD_KEY_KP_8:
                orbit_set_orientation(orbit, ORBIT_ORIENTATION_REAR);
            return;

            // front view
            case KBD_KEY_KP_2:
                orbit_set_orientation(orbit, ORBIT_ORIENTATION_FRONT);
            return;

            // left view
            case KBD_KEY_KP_4:
                orbit_set_orientation(orbit, ORBIT_ORIENTATION_LEFT);
            return;

            // right view
            case KBD_KEY_KP_6:
                orbit_set_orientation(orbit, ORBIT_ORIENTATION_RIGHT);
            return;

            // left side view
            case KBD_KEY_KP_1:
                orbit_set_orientation(orbit, ORBIT_ORIENTATION_LEFT_SIDE);
            return;

            // right side view
            case KBD_KEY_KP_3:
                orbit_set_orientation(orbit, ORBIT_ORIENTATION_RIGHT_SIDE);
            return;
        }
    }
}

static void on_mouse_move(struct mouse_event *e)
{
    struct camera_ctl *ctl = model_viewer_get_camera_ctl();

    if (ctl->type == CAMERA_CTL_ORBITAL)
    {
        struct camera_ctl_orbital *data = ctl->data;
        struct orbit *orbit = &data->orbit_dst;

        if (aio_hid_mouse_button_down(MOUSE_BUTTON_MIDDLE))
        {
            f32 sensitiviy = 0.20f;

            f32 polar = clamp(orbit->polar + e->y_delta * sensitiviy, -90 - FLT_EPSILON, 90 - FLT_EPSILON);
            f32 azimuth = orbit->azimuth + e->x_delta * sensitiviy;

            orbit_set_polar(orbit, polar);
            orbit_set_azimuth(orbit, azimuth);
        }
    }
}

static void on_mouse_wheel_spin(struct mouse_event *e)
{
    struct camera_ctl *ctl = model_viewer_get_camera_ctl();

    if (ctl->type == CAMERA_CTL_ORBITAL)
    {
        struct camera_ctl_orbital *data = ctl->data;
        struct orbit *orbit = &data->orbit_dst;

        f32 sensitiviy = 1.0;

        orbit_set_radius(orbit, orbit->radius - (f32)e->wy * sensitiviy);
    }
}

result_e model_viewer_init()
{
    csr_assert(!mv_ptr()->is_initialized);

    ////////////////////////////////////////

    // init renderer
    struct renderer *renderer = mv_renderer_ptr();
    check_result(renderer_init(renderer));

    mv_conf_ptr()->renderer = &renderer->conf;

    ////////////////////////////////////////

    // init scene
    struct scene *scene = mv_scene_ptr();
    check_result(scene_init(scene));

    // set root node
    struct mesh_node *node = &scene->root_node;
    {
        node->aabb = make_aabb_unit_cube();
        transform_identity(&node->transform);
    }

    // set model + controller
    mv_scene_ptr()->model = NULL;
    mv_scene_ptr()->model_ctl = NULL;

    // set camera + controller
    mv_scene_ptr()->camera = scene->cache.camera.main;
    mv_scene_ptr()->camera_ctl = &scene->cache.camera.controller.orbital;

    ////////////////////////////////////////

    struct hid_callbacks *hid_cbs = aio_get_hid_callbacks();
    check_ptr(hid_cbs);

    hid_cbs->on_mouse_move = on_mouse_move;
    hid_cbs->on_mouse_wheel_spin = on_mouse_wheel_spin;
    hid_cbs->on_kbd_key_down = on_kbd_key_press;

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

        // update camera
        {
            struct camera *camera = mv_scene_ptr()->camera;
            struct camera_ctl *camera_ctl = mv_scene_ptr()->camera_ctl;

            if (camera_ctl->update_cb) {
                camera_ctl->update_cb(camera, camera_ctl);
            }

            f32 aspect_ratio = screen_get_aspect_ratio(mv_renderer_ptr()->screen.rgpu);

            frame_data->mat_view = camera_get_view_matrix(camera);
            frame_data->mat_projection = camera_get_persp_projection_matrix(camera, aspect_ratio);
            frame_data->mat_projection_ortho = camera_get_ortho_projection_matrix(camera, aspect_ratio);
        }

        // update model
        {
            // update model transform (model_ctl)
            // update mesh_node hierarchy (compute mesh matrices)
        }
    }

    // draw frame
    renderer_tick(mv_renderer_ptr());
}

struct model* model_viewer_get_model()
{
    return mv_scene_ptr()->model;
}

struct camera* model_viewer_get_camera()
{
    return mv_scene_ptr()->camera;
}

struct camera_ctl* model_viewer_get_camera_ctl()
{
    return mv_scene_ptr()->camera_ctl;

error:
    return NULL;
}

void model_viewer_set_camera_ctl_type(enum camera_ctl_type type)
{
    struct scene_cache *cache = mv_scene_cache_ptr();

    switch (type)
    {
        case CAMERA_CTL_ORBITAL:
            mv_scene_ptr()->camera_ctl = &cache->camera.controller.orbital;
            break;

        case CAMERA_CTL_FIRST_PERSON:
            mv_scene_ptr()->camera_ctl = &cache->camera.controller.first_person;
            break;

        default:
            mv_scene_ptr()->camera_ctl = &cache->camera.controller.none;
    }

error:
    return;
}

struct mv_conf* model_viewer_get_conf()
{
    return &mv_ptr()->conf;
}
