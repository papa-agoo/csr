////////////////////////////////////////////////////////////////////////////////////////////////////

#include "model_viewer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

void mv_on_kbd_key_press(struct keyboard_event *e)
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
                orbit_set(orbit, make_vec3_zero(), 15, 75, 10); // FIXME mv_conf
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

void mv_on_mouse_move(struct mouse_event *e)
{
    struct camera *camera = model_viewer_get_camera();
    struct camera_ctl *ctl = model_viewer_get_camera_ctl();

    if (ctl->type == CAMERA_CTL_ORBITAL)
    {
        struct camera_ctl_orbital *data = ctl->data;
        struct orbit *orbit = &data->orbit_dst;

        f32 dx = e->x_delta;
        f32 dy = e->y_delta;

        // camera orientation (orbital)
        if (aio_hid_mouse_button_down(MOUSE_BUTTON_MIDDLE))
        {
            f32 sensitiviy = 0.20f;

            f32 polar = clamp(orbit->polar + dy * sensitiviy, -90 - FLT_EPSILON, 90 - FLT_EPSILON);
            f32 azimuth = orbit->azimuth + dx * sensitiviy;

            orbit_set_polar(orbit, polar);
            orbit_set_azimuth(orbit, azimuth);
        }

        // camera panning
        if (aio_hid_mouse_button_down(MOUSE_BUTTON_RIGHT))
        {
            // FIXME add option for interpolated grid point snapping (configurable distances: 0.5, 1.0, ...)
            f32 speed = aio_time_elapsed_delta() * orbit->radius * 0.1;

            struct mat44 m = mat44_rotate(camera_get_transform(camera)->rotation_euler);

            struct vec3 basis_x = mat44_mult_vec3(m, make_vec3_right());
            struct vec3 basis_y = mat44_mult_vec3(m, make_vec3_up());

            orbit->origin = vec3_add(orbit->origin, vec3_scale(basis_x, -dx * speed));
            orbit->origin = vec3_add(orbit->origin, vec3_scale(basis_y,  dy * speed));
        }
    }
}

void mv_on_mouse_wheel_spin(struct mouse_event *e)
{
    struct camera_ctl *ctl = model_viewer_get_camera_ctl();

    if (ctl->type == CAMERA_CTL_ORBITAL)
    {
        struct camera_ctl_orbital *data = ctl->data;
        struct orbit *orbit = &data->orbit_dst;

        f32 speed = orbit->radius * 0.1;

        orbit_set_radius(orbit, orbit->radius - (f32)e->wy * speed);
    }
}
