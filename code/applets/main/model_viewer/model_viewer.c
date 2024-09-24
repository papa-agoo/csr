////////////////////////////////////////////////////////////////////////////////////////////////////

#include "model_viewer.h"

#include "scene_priv.h"
#include "renderer_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct model_viewer
{
    bool is_initialized;

    struct {
        struct screen *rgpu;
        struct screen *rcpu;
    } screens;

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
#define mv_renderer_cache_ptr() (&mv_renderer_ptr()->cache)


static result_e _renderer_create();
static void _renderer_destroy();
static void _renderer_tick();

static result_e _scene_create();
static void _scene_destroy();
static void _scene_tick();

////////////////////////////////////////////////////////////////////////////////////////////////////
// public model viewer api
////////////////////////////////////////////////////////////////////////////////////////////////////
result_e model_viewer_init()
{
    csr_assert(!mv_ptr()->is_initialized);

    check_result(_renderer_create(), "could not create renderer");
    check_result(_scene_create(), "could not create scene");

    mv_ptr()->is_initialized = true;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void model_viewer_quit()
{
    csr_assert(mv_ptr()->is_initialized);

    _scene_destroy();
    _renderer_destroy();
}

void model_viewer_tick()
{
    _scene_tick();
    _renderer_tick();
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


////////////////////////////////////////////////////////////////////////////////////////////////////
// scene
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _scene_create()
{
    struct scene *scene = mv_scene_ptr();

    // init scene
    check_result(scene_init(scene), "could not init scene");

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

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _scene_destroy()
{
    struct scene *scene = mv_scene_ptr();

    // ...

    scene_quit(scene);
}

static void _scene_tick()
{
    // update camera
    {
        struct camera *camera = mv_scene_ptr()->camera;
        struct camera_ctl *camera_ctl = mv_scene_ptr()->camera_ctl;

        if (camera_ctl->update_cb) {
            camera_ctl->update_cb(camera, camera_ctl);
        }
    }

    // update model
    {
        // update model transform (model_ctl)
        // update mesh_node hierarchy (compute mesh matrices)
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// renderer
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _create_rgpu_screen(u32 width, u32 height, struct vec3 clear_color)
{
    check_expr(width > 0 && height > 0);

    struct screen_create_info create_info = {0};
    create_info.name = "GPU Renderer";

    create_info.surface.viewport.width = width;
    create_info.surface.viewport.height = height;
    create_info.surface.clear_values.color = make_vec4_3_1(clear_color, 1.0);

    struct screen* screen = aio_add_screen("rgpu", &create_info);
    check(screen, "could not create rgpu screen");

    mv_ptr()->screens.rgpu = screen;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_rcpu_screen(u32 width, u32 height, struct vec3 clear_color)
{
    check_expr(width > 0 && height > 0);

    struct screen_create_info create_info = {0};
    create_info.name = "CPU Renderer";

    create_info.surface.type = SCREEN_SURFACE_TYPE_CPU;
    create_info.surface.viewport.width = width;
    create_info.surface.viewport.height = height;
    create_info.surface.clear_values.color = make_vec4_3_1(clear_color, 1.0);

    struct screen *screen = aio_add_screen("rcpu", &create_info);
    check(screen, "could not create rcpu screen");

    mv_ptr()->screens.rcpu = screen;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _renderer_create()
{
    struct renderer *renderer = mv_renderer_ptr();

    // init config
    struct renderer_conf *conf = mv_conf_ptr()->renderer = &renderer->conf;
    renderer_conf_defaults(conf);

    // create screens
    check_result(_create_rgpu_screen(1280, 720, conf->color.background), "could not create rgpu screen");
    check_result(_create_rcpu_screen(640, 360, conf->color.background), "could not create rcpu screen");

    // init renderer
    check_result(renderer_init(renderer), "could not init renderer");

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _renderer_destroy()
{
    struct renderer *renderer = mv_renderer_ptr();

    // ...

    renderer_quit(renderer);
}

static void _renderer_rgpu_tick()
{
    // xgl_bind_descriptor_set(pipeline_layout, XGL_DESCRIPTOR_SET_TYPE_PER_FRAME, frame_data);
    // xgl_bind_descriptor_set(pipeline_layout, XGL_DESCRIPTOR_SET_TYPE_PER_PASS, pass_data);

    // draw environment
    {
        // ...
    }

    // draw meshes
    struct model *model = model_viewer_get_model();

    if (model)
    {
        struct vector *meshes = model->resources.mesh.meshes;

        for (u32 i = 0; i < vector_size(meshes); i++)
        {
            struct mesh *mesh = vector_get(meshes, i);

            // copy push data (model matrix, etc.)
            {
                struct shader_data_push *cpu = &mesh->push_data.cpu;
                struct shader_data_push *gpu = xgl_map_buffer(mesh->push_data.gpu);

                memcpy(gpu, cpu, sizeof(struct shader_data_push));
                xgl_unmap_buffer(mesh->push_data.gpu);
            }

            // draw primitives
            for (u32 i = 0; i < vector_size(mesh->primitives); i++)
            {
                struct mesh_primitive *primitive = vector_get(mesh->primitives, i);

                rgpu_set_material(primitive->material);
                rgpu_draw_mesh_primitive(primitive);
            }
        }
    }

    // draw gizmos
    {
        // grid
        struct mesh_gizmo *grid = &mv_renderer_cache_ptr()->gizmo.grid;
        {
            // rgpu_set_material(grid->primitive.material);
            // rgpu_draw_mesh_primitive(&grid->primitive);
        }

        // axes
        struct mesh_gizmo *axes = &mv_renderer_cache_ptr()->gizmo.axes;
        {
            // struct xgl_viewport axes_vp = {0};
            // xgl_set_viewports(1, &axes_vp);

            // rgpu_set_material(axes->primitive.material);
            // rgpu_draw_mesh_primitive(&axes->primitive);
        }
    }
}

static void _renderer_rcpu_tick()
{
    // ...
}

static void _renderer_tick()
{
    // update shader resources
    {
        struct shader_data_frame *cpu = NULL;
        struct shader_data_frame *gpu = NULL;

        // ...
    }

    // build render structures (suitable for gpu/cpu renderers)
    {
        // ...
    }

    // gpu renderer
    if (screen_begin(mv_ptr()->screens.rgpu, SCREEN_SURFACE_TYPE_GPU))
    {
        _renderer_rgpu_tick();

        screen_end();
    }

    // cpu renderer
    if (screen_begin(mv_ptr()->screens.rcpu, SCREEN_SURFACE_TYPE_CPU))
    {
        _renderer_rcpu_tick();

        screen_end();
    }
}
