////////////////////////////////////////////////////////////////////////////////////////////////////

#include "model_viewer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////


struct model_viewer
{
    bool is_initialized;

    struct {
        struct screen *rgpu;
        struct screen *rcpu;
    } screens;

    struct {
        struct model *model;
        struct model_ctl *model_ctl;

        struct camera *camera;
        struct camera_ctl *camera_ctl;
    } scene;

    struct mv_resources resources;
    struct mv_renderer renderer;
    struct mv_conf conf;
};

static struct model_viewer g_mv = {0};

#define mv_ptr() (&g_mv)
#define mv_conf_ptr() (&mv_ptr()->conf)

#define mv_scene_ptr() (&mv_ptr()->scene)
#define mv_renderer_ptr() (&mv_ptr()->renderer)
#define mv_resources_ptr() (&mv_ptr()->resources)


////////////////////////////////////////////////////////////////////////////////////////////////////
// public model viewer api
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _create_scene();
static result_e _create_renderer();
static result_e _create_resources();

static result_e _create_screens()
{
    struct vec4 clear_color = make_vec4_3_1(mv_conf_ptr()->renderer.color.background, 1.0f);

    // rgpu screen
    {
        struct vec2 size = make_vec2(1280, 720);

        struct screen_create_info create_info = {0};
        create_info.name = "GPU Renderer";

        create_info.surface.viewport.width = size.w;
        create_info.surface.viewport.height = size.h;
        create_info.surface.clear_values.color = clear_color;

        struct screen* screen = aio_add_screen("rgpu", &create_info);
        check(screen, "could not create rgpu screen");

        mv_ptr()->screens.rgpu = screen;
    }

    // rcpu screen
    {
        struct vec2 size = make_vec2(640, 360);

        struct screen_create_info create_info = {0};
        create_info.name = "CPU Renderer";

        create_info.surface.type = SCREEN_SURFACE_TYPE_CPU;
        create_info.surface.viewport.width = size.w;
        create_info.surface.viewport.height = size.h;
        create_info.surface.clear_values.color = clear_color;

        struct screen *screen = aio_add_screen("rcpu", &create_info);
        check(screen, "could not create rcpu screen");

        mv_ptr()->screens.rcpu = screen;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e model_viewer_init()
{
    csr_assert(!mv_ptr()->is_initialized);

    ////////////////////////////////////////

    mv_renderer_conf_defaults(&mv_conf_ptr()->renderer);

    check_result(_create_screens(), "could not create screens");
    check_result(_create_scene(), "could not create scene");
    // check_result(_create_renderer(), "could not create renderer");
    // check_result(_create_resources(), "could not create resources");

    ////////////////////////////////////////

    mv_ptr()->is_initialized = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void model_viewer_quit()
{
    csr_assert(mv_ptr()->is_initialized);
}

void model_viewer_tick()
{
    // update model
    {
        // ...
    }

    // update camera
    {
        struct camera *camera = mv_scene_ptr()->camera;
        struct camera_ctl *camera_ctl = mv_scene_ptr()->camera_ctl;

        if (camera_ctl->update_cb) {
            camera_ctl->update_cb(camera, camera_ctl);
        }
    }

    // prepare render data
    {
        // ...
    }

    ////////////////////////////////////////

    if (screen_begin(mv_ptr()->screens.rgpu, SCREEN_SURFACE_TYPE_GPU))
    {
        // rgpu render ...

        screen_end();
    }

    if (screen_begin(mv_ptr()->screens.rcpu, SCREEN_SURFACE_TYPE_CPU))
    {
        // rcpu render ...

        screen_end();
    }
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
    switch (type)
    {
        case CAMERA_CTL_ORBITAL:
            mv_scene_ptr()->camera_ctl = &mv_resources_ptr()->camera.controller.orbital;
            break;

        case CAMERA_CTL_FIRST_PERSON:
            mv_scene_ptr()->camera_ctl = &mv_resources_ptr()->camera.controller.first_person;
            break;

        default:
            mv_scene_ptr()->camera_ctl = &mv_resources_ptr()->camera.controller.none;
    }

error:
    return;
}

struct mv_conf* model_viewer_get_conf()
{
    return &mv_ptr()->conf;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// scene
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _camera_ctl_orbital_update_cb(struct camera *camera, struct camera_ctl *ctl)
{
    check_ptr(camera);
    check_ptr(ctl);

    check_expr(ctl->type == CAMERA_CTL_ORBITAL);

    struct camera_ctl_orbital *data = ctl->data;
    check_ptr(data);

    struct transform *transform = camera_get_transform(camera);
    check_ptr(transform);

    struct orbit *orbit = &data->orbit_src;

    data->orbit_dst.azimuth = data->orbit_dst.azimuth + data->animate;

    if (data->interpolate)
    {
        f32 scale = 6.0; // FIXME move to config
        f32 value = scale * aio_time_elapsed_delta();

        struct orbit *src = &data->orbit_src;
        struct orbit *dst = &data->orbit_dst;

        src->azimuth = lerp(value, src->azimuth, dst->azimuth);
        src->polar = lerp(value, src->polar, dst->polar);
        src->radius = lerp(value, src->radius, dst->radius);
    }
    else
    {
        data->orbit_src = data->orbit_dst;
    }

    transform_lookat_from(transform, orbit_get_coords(orbit), orbit->origin);

error:
    return;
}

static void _camera_ctl_first_person_update_cb(struct camera *camera, struct camera_ctl *ctl)
{
    // ...
}

static result_e _create_scene()
{
    // create dummy model
    {
        struct model_create_info create_info = {0};
        create_info.name = make_string("Dummy");

        struct model *model = model_create(&create_info);
        check_ptr(model);

        mv_resources_ptr()->model.dummy = model;
    }

    // create main camera
    {
        struct camera_create_info create_info = {0};
        create_info.name = make_string("main");
        create_info.mode = CAMERA_PROJECTION_PERSPECTIVE;

        struct camera *camera = camera_create(&create_info);
        check(camera, "could not create main camera");

        mv_resources_ptr()->camera.main = camera;
    }

    // create dummy controller
    {
        struct camera_ctl ctl = {0};
        ctl.type = CAMERA_CTL_NONE;

        mv_resources_ptr()->camera.controller.none = ctl;
    }

    // create orbital controller
    {
        f32 azimuth = 75.0;
        f32 polar = 15.0;
        f32 radius = 5.0;

        static struct camera_ctl_orbital data = {0};
        data.orbit_dst.azimuth = azimuth;
        data.orbit_dst.polar = polar;
        data.orbit_dst.radius = radius;
        data.orbit_src = data.orbit_dst;
        data.interpolate = true;

        struct camera_ctl ctl = {0};
        ctl.type = CAMERA_CTL_ORBITAL;
        ctl.update_cb = _camera_ctl_orbital_update_cb;
        ctl.data = &data;

        mv_resources_ptr()->camera.controller.orbital = ctl;
    }

    // create first person controller
    {
        struct camera_ctl ctl = {0};
        ctl.type = CAMERA_CTL_FIRST_PERSON;
        ctl.update_cb = _camera_ctl_first_person_update_cb;

        // ...

        mv_resources_ptr()->camera.controller.first_person = ctl;
    }

    // set default camera + controller
    mv_scene_ptr()->model = mv_resources_ptr()->model.dummy;
    mv_scene_ptr()->camera = mv_resources_ptr()->camera.main;
    mv_scene_ptr()->camera_ctl = &mv_resources_ptr()->camera.controller.orbital;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// renderer
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _create_renderer()
{
    return RC_SUCCESS;

error:
    return RC_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// resources
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _create_axes_gizmo()
{
    // struct {
    //     struct vec3 position;
    //     struct vec3 color;
    // } vertex;

    // u32 vertex_format = VERTEX_FORMAT_POSITION | VERTEX_FORMAT_COLOR;

    // ////////////////////////////////////////

    // // 3 axes, 2 vertices for each axis
    // struct vector *vertices = vector_create(3 * 2, sizeof(vertex));
    // check_mem(vertices);

    // // x axis
    // vertex.color = mv_renderer_conf_ptr()->color.axis_x;

    // vertex.position = make_vec3(0, 0, 0);
    // vector_push_back(vertices, vertex);

    // vertex.position = make_vec3_x_axis();
    // vector_push_back(vertices, vertex);

    // // y axis
    // vertex.color = mv_renderer_conf_ptr()->color.axis_y;

    // vertex.position = make_vec3(0, 0, 0);
    // vector_push_back(vertices, vertex);

    // vertex.position = make_vec3_y_axis();
    // vector_push_back(vertices, vertex);

    // // z axis
    // vertex.color = mv_renderer_conf_ptr()->color.axis_z;

    // vertex.position = make_vec3(0, 0, 0);
    // vector_push_back(vertices, vertex);

    // vertex.position = make_vec3_z_axis();
    // vector_push_back(vertices, vertex);

    // ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_grid_gizmo(f32 size_qm)
{
    check_expr(size_qm >= 1);

    // f32 e = size_qm / 2.0;
    // f32 step_size = e / e;

    // ////////////////////////////////////////

    // struct {
    //     struct vec3 position;
    //     struct vec3 color;
    // } vertex;

    // u32 num_lines = (2 * (size_qm + 1)) + 2;

    // struct vector *vertices = vector_create(num_lines * 2, sizeof(vertex));
    // check_mem(vertices);

    // ////////////////////////////////////////

    // // generate grid lines
    // for (f32 i = -e; i <= e; i += step_size)
    // {
    //     vertex.color = mv_renderer_conf_ptr()->color.grid;

    //     // along x axis
    //     {
    //         // p1
    //         vertex.position = make_vec3(-e, 0, i);
    //         vector_push_back(vertices, vertex);

    //         // p2
    //         vertex.position = (i == 0) ? make_vec3(0, 0, 0) : make_vec3(e, 0, i);
    //         vector_push_back(vertices, vertex);
    //     }

    //     // along z axis
    //     {
    //         // p1
    //         vertex.position = make_vec3(i, 0, -e);
    //         vector_push_back(vertices, vertex);

    //         // p2
    //         vertex.position = (i == 0) ? make_vec3(0, 0, 0) : make_vec3(i, 0, e);
    //         vector_push_back(vertices, vertex);
    //     }
    // }

    // // colored x axis from origin to e
    // {
    //     vertex.color = mv_renderer_conf_ptr()->color.axis_x;

    //     vertex.position = make_vec3(0, 0, 0);
    //     vector_push_back(vertices, vertex);

    //     vertex.position = make_vec3(e, 0, 0);
    //     vector_push_back(vertices, vertex);
    // }

    // // colored z axis from origin to e
    // {
    //     vertex.color = mv_renderer_conf_ptr()->color.axis_z;

    //     vertex.position = make_vec3(0, 0, 0);
    //     vector_push_back(vertices, vertex);

    //     vertex.position = make_vec3(0, 0, e);
    //     vector_push_back(vertices, vertex);
    // }

    // ////////////////////////////////////////

    // struct aabb aabb = make_aabb(make_vec3(-e, 0, -e), make_vec3(e, 0, e));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_resources()
{
    // create gizmos
    check_result(_create_axes_gizmo(), "could not create axes gizmo");
    check_result(_create_grid_gizmo(100), "could not create grid gizmo");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
