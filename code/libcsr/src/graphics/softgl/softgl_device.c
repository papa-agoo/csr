////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static bool _check_shader_runtime_valid(struct softgl_shader *shader)
{
    // ...

    return true;

error:
    return false;
}

result_e softgl_create_shader(struct softgl_shader_create_info* info, softgl_shader* p_shader)
{
    check_ptr(info);
    check_ptr(p_shader);

    check_expr(info->type != SOFTGL_SHADER_TYPE_UNKNOWN);
    check_ptr(info->vertex_shader_ptr);
    check_ptr(info->fragment_shader_ptr);

    ////////////////////////////////////////

    struct softgl_shader shader = {0};
    shader.name = string_is_valid(info->name) ? info->name : make_string("<no name>");
    shader.type = info->type;
    shader.vertex_shader_ptr = info->vertex_shader_ptr;
    shader.fragment_shader_ptr = info->fragment_shader_ptr;

    check_expr(_check_shader_runtime_valid(&shader));

    ////////////////////////////////////////

    p_shader->handle = object_pool_alloc(softgl_storage_ptr()->shaders, &shader);
    check_guid(p_shader->handle);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void softgl_destroy_shader(softgl_shader p_shader)
{
    object_pool_free(softgl_storage_ptr()->shaders, p_shader.handle);
}

result_e softgl_create_pipeline(struct softgl_pipeline_create_info* info, softgl_pipeline* p_pipeline)
{
    check_ptr(info);
    check_ptr(p_pipeline);

    check_ptr(info->ia_state);
    check_ptr(info->shader_state);
    check_ptr(info->input_layout);

    ////////////////////////////////////////

    check_expr(info->ia_state->topology != SOFTGL_TOPOLOGY_UNKNOWN);

    struct softgl_shader *shader = object_pool_get(softgl_storage_ptr()->shaders, info->shader_state->shader.handle);
    check_ptr(shader);

    ////////////////////////////////////////

    struct softgl_pipeline pipeline = {0};
    pipeline.name = string_is_valid(info->name) ? info->name : make_string("<no name>");

    pipeline.topology = info->ia_state->topology;
    pipeline.shader = shader;

    if (info->depth_stencil_state) {
        pipeline.depth_stencil_state = *info->depth_stencil_state;
    }

    // .. rasterizer state
    // .. color blend state

    struct softgl_input_layout *input_layout = info->input_layout;
    {
        check_ptr(input_layout->attrib_inputs);
        check_expr(input_layout->attrib_input_count > 0);

        ////////////////////////////////////////

        // vertex attrib inputs
        pipeline.attrib_inputs = vector_create(input_layout->attrib_input_count, sizeof(struct softgl_vertex_attrib_input));
        check_ptr(pipeline.attrib_inputs);

        for (u32 i = 0; i < input_layout->attrib_input_count; i++) {
            vector_push_back(pipeline.attrib_inputs, input_layout->attrib_inputs[i]);
        }
    }

    ////////////////////////////////////////

    p_pipeline->handle = object_pool_alloc(softgl_storage_ptr()->pipelines, &pipeline);
    check_guid(p_pipeline->handle);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void softgl_destroy_pipeline(softgl_pipeline p_pipeline)
{
    struct softgl_storage *storage = softgl_storage_ptr();

    struct softgl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline.handle);
    check_ptr(pipeline);

    if (pipeline->attrib_inputs) {
        vector_destroy(pipeline->attrib_inputs);
    }

    object_pool_free(storage->pipelines, p_pipeline.handle);

error:
    return;
}
