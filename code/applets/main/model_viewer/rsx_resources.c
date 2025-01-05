////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rsx_priv.h"

#include "rsx/rgpu_priv.h"
#include "rsx/rcpu_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
// materials
////////////////////////////////////////////////////////////////////////////////////////////////////
struct rsx_material* rsx_material_create(struct rsx_material_create_info *info)
{
    clog_warn("not impl. yet");

    return NULL;
}

struct rsx_material* rsx_material_copy(struct rsx_material *material)
{
    clog_warn("not impl. yet");

    return NULL;
}

void rsx_material_destroy(struct rsx_material* material)
{
    clog_warn("not impl. yet");
}

struct rsx_material* rsx_material_find_suitable(u32 vertex_format)
{
    clog_warn("not impl. yet");

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// meshes
////////////////////////////////////////////////////////////////////////////////////////////////////
static result_e _create_shader_resource_object(struct rsx_shader_resource_object *resource)
{
    check_ptr(resource);

    // uniform buffer
    struct rsx_uniform_buffer_object *ubo = &resource->data;
    {
        shader_data_object_init(&ubo->cpu);

        struct xgl_buffer_create_info info = {0};
        info.usage_flags = XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        info.byte_length = sizeof(ubo->cpu);
        info.data = &ubo->cpu;

        check_result(xgl_create_buffer(&info, &ubo->gpu));
    }

    // resource binding
    struct rsx_shader_resource_binding *binding = &resource->binding;
    {
        // copy layout info from cache
        struct rgpu_cache *cache = rsx_rgpu_cache_ptr();

        binding->ds_layout = cache->ds_layout.frame;
        binding->pipeline_layout = cache->pipeline_layout.main;

        check_result(xgl_create_descriptor_set(binding->ds_layout, &binding->ds));

        // update descriptor set
        struct xgl_buffer_descriptor descriptor = {0};
        descriptor.binding = 0;
        descriptor.buffer = ubo->gpu;

        struct xgl_descriptor_set_update_info update_info = {0};
        update_info.buffer_descriptors = &descriptor;
        update_info.buffer_descriptor_count = 1;

        check_result(xgl_update_descriptor_set(binding->ds, &update_info));
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static void _destroy_shader_resource_object(struct rsx_shader_resource_object *resource)
{
    check_ptr(resource);

    xgl_destroy_buffer(resource->data.gpu);
    xgl_destroy_descriptor_set(resource->binding.ds);

error:
    return;
}

static result_e _update_mesh_info(struct rsx_mesh *mesh, struct rsx_mesh_primitive_data *primitives, u32 count)
{
    struct rsx_mesh_info *info = &mesh->info;

    for (u32 i = 0; i < count; i++)
    {
        struct rsx_mesh_primitive_data *data = primitives + i;

        check_expr(data->vertex_format != 0);
        check_expr(data->vertex_data && data->vertex_count > 0);

        u32 vertex_stride = vertex_format_get_stride(data->vertex_format);

        info->primitive_count++;

        info->vertices_count += data->vertex_count;
        info->vertices_byte_length += data->vertex_count * vertex_stride;

        info->indices_count += data->index_count;
        info->indices_byte_length += data->index_count * sizeof(u32);

        info->total_byte_length += info->vertices_byte_length;
        info->total_byte_length += info->indices_byte_length;
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _allocate_mesh_storage(struct rsx_mesh *mesh)
{
    struct rsx_mesh_info *info = &mesh->info;
    struct rsx_mesh_geometry *geometry = &mesh->geometry;

    // allocate storage for mesh primitives
    geometry->primitives = vector_create(info->primitive_count, sizeof(struct rsx_mesh_primitive));
    check_ptr(geometry->primitives);

    // vertex buffer
    {
        // cpu storage
        geometry->vertex_buffer.cpu = arena_push(geometry->arena, info->vertices_byte_length);
        check_ptr(geometry->vertex_buffer.cpu);

        // gpu storage
        struct xgl_buffer_create_info vb_info = {0};
        vb_info.byte_length = info->vertices_byte_length;
        vb_info.usage_flags = XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        check_result(xgl_create_buffer(&vb_info, &geometry->vertex_buffer.gpu));
    }

    // index buffer
    if (info->indices_byte_length > 0)
    {
        // cpu storage
        geometry->index_buffer.cpu = arena_push(geometry->arena, info->indices_byte_length);
        check_ptr(geometry->index_buffer.cpu);

        // gpu storage
        struct xgl_buffer_create_info ib_info = {0};
        ib_info.byte_length = info->indices_byte_length;
        ib_info.usage_flags = XGL_BUFFER_USAGE_INDEX_BUFFER_BIT;

        check_result(xgl_create_buffer(&ib_info, &geometry->index_buffer.gpu));
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static result_e _create_mesh_primitives(struct rsx_mesh *mesh, struct rsx_mesh_primitive_data *primitives, u32 count)
{
    check_ptr(mesh);
    check_ptr(primitives);
    check_expr(count > 0);

    ////////////////////////////////////////

    // update mesh info
    check_result(_update_mesh_info(mesh, primitives, count));

    // allocate storage
    check_result(_allocate_mesh_storage(mesh));

    ////////////////////////////////////////

    struct rsx_mesh_geometry *geometry = &mesh->geometry;

    u64 vertex_data_offset = 0;
    u64 index_data_offset = 0;

    // copy data to cpu storage
    for (u32 i = 0; i < count; i++)
    {
        struct rsx_mesh_primitive_data *data = primitives + i;

        check_expr(data->material != NULL); // FIXME rsx_material_find_suitable()

        u32 vertex_stride = vertex_format_get_stride(data->vertex_format);

        // build mesh primitive
        struct rsx_mesh_primitive primitive = {0};
        {
            primitive.name = data->name;

            primitive.vertex_format = data->vertex_format;
            primitive.vertex_stride = vertex_stride;

            primitive.vertices.offset_bytes = vertex_data_offset;
            primitive.vertices.count = data->vertex_count;

            primitive.indices.offset_bytes = index_data_offset;
            primitive.indices.count = data->index_count;

            primitive.mesh = mesh;
            primitive.material = data->material;

            vector_push_back(geometry->primitives, primitive);
        }

        // copy vertex data : intermediate -> final storage
        u64 vertex_data_size = data->vertex_count * vertex_stride;
        {
            void *vertex_data = geometry->vertex_buffer.cpu + vertex_data_offset;

            memcpy(vertex_data, data->vertex_data, vertex_data_size);

            vertex_data_offset += vertex_data_size;
        }

        // copy index data : intermediate -> final storage
        u64 index_data_size = data->index_count * sizeof(u32);

        if (index_data_size > 0)
        {
            void *index_data = geometry->index_buffer.cpu + index_data_offset;

            memcpy(index_data, data->index_data, index_data_size);

            index_data_offset += index_data_size;
        }
    }

    ////////////////////////////////////////

    // cpu storage is ready, copy data to gpu storage
    struct rsx_mesh_info *info = &mesh->info;
    {
        // vertex data
        check_result(xgl_update_buffer(geometry->vertex_buffer.gpu, 0, info->vertices_byte_length, geometry->vertex_buffer.cpu));

        // index data
        if (info->indices_byte_length > 0) {
            check_result(xgl_update_buffer(geometry->index_buffer.gpu, 0, info->indices_byte_length, geometry->index_buffer.cpu));
        }
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

struct rsx_mesh* rsx_mesh_create(struct rsx_mesh_create_info *info)
{
    check_ptr(info);
    check_ptr(info->arena);

    check_expr(info->primitives && info->primitive_count > 0);

    struct rsx_mesh *mesh = calloc(1, sizeof(struct rsx_mesh));
    check_mem(mesh);

    mesh->name = info->name;
    mesh->geometry.arena = info->arena;

    check_result(_create_mesh_primitives(mesh, info->primitives, info->primitive_count));
    check_result(_create_shader_resource_object(&mesh->shader_data));

    return mesh;

error:
    return NULL;
}

struct rsx_mesh* rsx_mesh_copy(struct rsx_mesh *mesh)
{
    clog_warn("not impl. yet");

    return NULL;
}

void rsx_mesh_destroy(struct rsx_mesh* mesh)
{
    check_ptr(mesh);

    struct rsx_mesh_geometry *geometry = &mesh->geometry;
    {
        vector_destroy(geometry->primitives);

        xgl_destroy_buffer(geometry->vertex_buffer.gpu);

        if (mesh->info.indices_byte_length > 0) {
            xgl_destroy_buffer(geometry->index_buffer.gpu);
        }
    }

    _destroy_shader_resource_object(&mesh->shader_data);

error:
    return;
}

struct rsx_mesh* rsx_mesh_create_test_triangle()
{
    struct rsx_material *material = &rsx_get_render_data()->material.debug_colors;

    ////////////////////////////////////////

    struct vertex_1p1c vertices[] = {
        {.position = make_vec3(-1, -1, 0), .color = make_vec3(1, 0, 0)},
        {.position = make_vec3( 0,  1, 0), .color = make_vec3(0, 1, 0)},
        {.position = make_vec3( 1, -1, 0), .color = make_vec3(0, 0, 1)},
    };

    struct rsx_mesh_primitive_data primitive = {0};
    primitive.name = make_string("primitive.triangle");
    primitive.vertex_format = VERTEX_FORMAT_1P_1C;
    primitive.vertex_data = vertices;
    primitive.vertex_count = sizeof(vertices) / sizeof(struct vertex_1p1c);
    primitive.material = material;

    ////////////////////////////////////////

    struct rsx_mesh_create_info info = {0};
    info.name = make_string("Test Triangle");
    info.arena = rsx_arena_ptr();
    info.primitives = &primitive;
    info.primitive_count = 1;

    return rsx_mesh_create(&info);
}

struct rsx_mesh* rsx_mesh_create_test_box()
{
    struct rsx_material *material = &rsx_get_render_data()->material.debug_colors;

    ////////////////////////////////////////

    struct aabb aabb = make_aabb_unit_cube();

    //   [tb] --- [tc]
    //  /           /
    // [ta] --- [td]
    //
    // top points (clock wise order)

    struct vertex_1p1c ta = {.position = make_vec3(aabb.min.x, aabb.max.y, aabb.max.z), .color = make_vec3(0, 1, 0)};
    struct vertex_1p1c tb = {.position = make_vec3(aabb.min.x, aabb.max.y, aabb.min.z), .color = make_vec3(0, 0, 1)};
    struct vertex_1p1c tc = {.position = make_vec3(aabb.max.x, aabb.max.y, aabb.min.z), .color = make_vec3(1, 0, 1)};
    struct vertex_1p1c td = {.position = make_vec3(aabb.max.x, aabb.max.y, aabb.max.z), .color = make_vec3(1, 1, 0)};

    //   [bb] --- [bc]
    //  /           /
    // [ba] --- [bd]
    //
    // bottom points (clock wise order)

    struct vertex_1p1c ba = {.position = make_vec3(aabb.min.x, aabb.min.y, aabb.max.z), .color = make_vec3(1, 0, 0)};
    struct vertex_1p1c bb = {.position = make_vec3(aabb.min.x, aabb.min.y, aabb.min.z), .color = make_vec3(1, 0, 0)};
    struct vertex_1p1c bc = {.position = make_vec3(aabb.max.x, aabb.min.y, aabb.min.z), .color = make_vec3(1, 0, 0)};
    struct vertex_1p1c bd = {.position = make_vec3(aabb.max.x, aabb.min.y, aabb.max.z), .color = make_vec3(1, 0, 0)};

    ////////////////////////////////////////

    struct vertex_1p1c vertices[] = {
        ta, tb, tc, td, // 0 - 3
        ba, bb, bc, bd, // 4 - 7
    };

    u32 indices[] = {
        0, 1, 2, 2, 3, 0, // [ta, tb, tc], [tc, td, ta] top face
        4, 5, 6, 6, 7, 4, // [ba, bb, bc], [bc, bd, ba] bottom face

        4, 0, 3, 3, 7, 4, // [ba, ta, td], [td, bd, ba] front face
        5, 1, 2, 2, 6, 5, // [bb, tb, tc], [tc, bc, bb] back face

        5, 1, 0, 0, 4, 5, // [bb, tb, ta], [ta, ba, bb ] left face
        7, 3, 2, 2, 6, 7, // [bd, td, tc], [tc, bc, bd] right face
    };

    struct rsx_mesh_primitive_data primitive = {0};
    primitive.name = make_string("primitive.box");
    primitive.vertex_format = VERTEX_FORMAT_1P_1C; // VERTEX_FORMAT_1P_1N_1UV;
    primitive.vertex_data = vertices;
    primitive.vertex_count = sizeof(vertices) / sizeof(struct vertex_1p1c);
    primitive.index_data = indices;
    primitive.index_count = sizeof(indices) / sizeof(u32);
    primitive.material = material;

    ////////////////////////////////////////

    struct rsx_mesh_create_info info = {0};
    info.name = make_string("Test Box");
    info.arena = rsx_arena_ptr();
    info.primitives = &primitive;
    info.primitive_count = 1;

    return rsx_mesh_create(&info);
}
