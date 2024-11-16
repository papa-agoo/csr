////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

void xgl_reset_pipeline_state_impl()
{
    struct gl_state *state = gl_state_ptr();
    state->pso = NULL;

    glUseProgram(0);
    glDisable(GL_RASTERIZER_DISCARD);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    // _apply_pipeline_state(driver->default_pipeline_state)

error:
    return;
}

void xgl_reset_resource_bindings_impl()
{
    // framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // vertex array (including all vertex / index buffers)
    glBindVertexArray(0);

    // textures
    // samplers
    // uniform buffers
    // storage buffers

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

void xgl_clear_color_buffer_impl(guid p_framebuffer, u32 buffer_idx, f32 *rgba)
{
    check_ptr(rgba);

    struct gl_storage *storage = gl_storage_ptr();

    struct gl_framebuffer *framebuffer = object_pool_get(storage->framebuffers, p_framebuffer);
    check_ptr(framebuffer);

    GL_CALL( glClearNamedFramebufferfv(framebuffer->id, GL_COLOR, buffer_idx, rgba) );

error:
    return;
}

void xgl_clear_depth_stencil_buffer_impl(guid p_framebuffer, f32 depth, u32 stencil)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_framebuffer *framebuffer = object_pool_get(storage->framebuffers, p_framebuffer);
    check_ptr(framebuffer);

    GL_CALL( glClearNamedFramebufferfi(framebuffer->id, GL_DEPTH_STENCIL, 0, depth, stencil) );

error:
    return;
}

void xgl_set_viewport_impl(struct xgl_viewport vp)
{
    // FIXME sanity checks?

    GL_CALL( glViewport(vp.x, vp.y, vp.width, vp.height) );
    GL_CALL( glDepthRange(vp.min_depth, vp.max_depth) );

error:
    return;
}

void xgl_set_scissor_rect_impl(struct xgl_rect sr)
{
    // FIXME sanity checks?

    GL_CALL( glScissor(sr.x, sr.y, sr.width, sr.height) );

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

CSR_INLINE void _pipeline_apply_depth_state(struct gl_depth_state *p_ds)
{
    check_ptr(p_ds);

    if (p_ds->enable_test)
    {
        GL_CALL( glEnable(GL_DEPTH_TEST) );

        GL_CALL( glDepthMask(p_ds->enable_write) );
        GL_CALL( glDepthFunc(p_ds->compare_op) );
    }
    else
    {
        GL_CALL( glDisable(GL_DEPTH_TEST) );
    }

error:
    return;
}

CSR_INLINE void _apply_stencil_face_state(GLenum face, struct gl_stencil_op_state *state)
{
    GL_CALL( glStencilMaskSeparate(face, state->write_mask) );
    GL_CALL( glStencilFuncSeparate(face, state->compare_op, state->ref_value, state->compare_mask) );
    GL_CALL( glStencilOpSeparate(face, state->sfail_op, state->spass_zfail_op, state->spass_zpass_op) );

error:
    return;
}

CSR_INLINE void _pipeline_apply_stencil_state(struct gl_stencil_state *p_ss)
{
    check_ptr(p_ss);

    if (p_ss->enable)
    {
        GL_CALL( glEnable(GL_STENCIL_TEST) );

        _apply_stencil_face_state(GL_FRONT, &p_ss->front);
        _apply_stencil_face_state(GL_BACK, &p_ss->back);
    }
    else
    {
        GL_CALL( glDisable(GL_STENCIL_TEST) );
    }

error:
    return;
}

CSR_INLINE void _pipeline_apply_rasterizer_state(struct gl_rasterizer_state *p_rs)
{
    check_ptr(p_rs);

    ////////////////////////////////////////

    if (p_rs->discard_primitives) {
        GL_CALL( glEnable(GL_RASTERIZER_DISCARD) );
    }
    else {
        GL_CALL( glDisable(GL_RASTERIZER_DISCARD) );
    }

    ////////////////////////////////////////

    if (p_rs->enable_culling)
    {
        GL_CALL( glEnable(GL_CULL_FACE) );

        GL_CALL( glCullFace(p_rs->cull_mode) );
        GL_CALL( glFrontFace(p_rs->front_face) );
    }
    else
    {
        GL_CALL( glDisable(GL_CULL_FACE) );
    }

    ////////////////////////////////////////

    GL_CALL( glPolygonMode(GL_FRONT_AND_BACK, p_rs->polygon_mode) );

    GL_CALL( glPointSize(p_rs->point_size) );
    GL_CALL( glLineWidth(p_rs->line_width) );

    (p_rs->smooth_points) ? GL_CALL( glEnable(GL_POINT_SMOOTH) ) : GL_CALL( glDisable(GL_POINT_SMOOTH) );
    (p_rs->smooth_lines) ? GL_CALL( glEnable(GL_LINE_SMOOTH) ) : GL_CALL( glDisable(GL_LINE_SMOOTH) );

    ////////////////////////////////////////

error:
    return;
}

CSR_INLINE void _pipeline_apply_color_blend_state(struct gl_color_blend_state *p_bs)
{
    check_ptr(p_bs);

    if (p_bs->enable)
    {
        GL_CALL( glEnable(GL_BLEND) );

        GL_CALL( glBlendFuncSeparate(p_bs->src_color_blend_factor, p_bs->dst_color_blend_factor,
            p_bs->src_alpha_blend_factor, p_bs->dst_alpha_blend_factor) );

        GL_CALL( glBlendEquationSeparate(p_bs->color_blend_op, p_bs->alpha_blend_op) );
    }
    else
    {
        GL_CALL( glDisable(GL_BLEND) );
    }

error:
    return;
}

void xgl_bind_pipeline_impl(guid p_pipeline)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline);
    check_ptr(pipeline);

    struct gl_state *state = gl_state_ptr();
    state->pso = pipeline;

    GL_CALL( glUseProgram(pipeline->shader_program) );
    GL_CALL( glBindVertexArray(pipeline->vertex_arrays) );

    _pipeline_apply_depth_state(&pipeline->depth_state);
    _pipeline_apply_stencil_state(&pipeline->stencil_state);
    _pipeline_apply_rasterizer_state(&pipeline->rasterizer_state);
    _pipeline_apply_color_blend_state(&pipeline->color_blend_state);

error:
    return;
}

void xgl_bind_framebuffer_impl(guid p_framebuffer)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_framebuffer *framebuffer = object_pool_get(storage->framebuffers, p_framebuffer);
    check_ptr(framebuffer);

    GL_CALL( glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id) );

error:
    return;
}

void xgl_bind_index_buffer_impl(guid p_buffer)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_buffer *buffer = object_pool_get(storage->buffers, p_buffer);
    check_ptr(buffer);

    struct gl_state *state = gl_state_ptr();
    state->ebo = &buffer->id;

    GL_CALL( glVertexArrayElementBuffer(state->pso->vertex_arrays, buffer->id) );

error:
    return;
}

void xgl_bind_vertex_buffer_impl(guid p_buffer, u32 binding, u32 offset, u32 stride)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_buffer *buffer = object_pool_get(storage->buffers, p_buffer);
    check_ptr(buffer);

    struct gl_state *state = gl_state_ptr();

    struct gl_pipeline *pipeline = state->pso;
    check_ptr(pipeline);

    GL_CALL( glVertexArrayVertexBuffer(pipeline->vertex_arrays, binding, buffer->id, offset, stride) );

error:
    return;
}

void xgl_bind_uniform_buffer_impl(guid p_buffer, u32 binding)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_buffer *buffer = object_pool_get(storage->buffers, p_buffer);
    check_ptr(buffer);

    GL_CALL( glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer->id) );

error:
    return;
}

void xgl_bind_texture_impl(guid p_texture, u32 texture_unit)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_texture *texture = object_pool_get(storage->textures, p_texture);
    check_ptr(texture);

    GL_CALL( glBindTextureUnit(texture_unit, texture->id) );

error:
    return;
}

void xgl_bind_sampler_impl(guid p_sampler, u32 texture_unit)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_sampler *sampler = object_pool_get(storage->samplers, p_sampler);
    check_ptr(sampler);

    GL_CALL( glBindSampler(texture_unit, sampler->id) );

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

void xgl_draw_impl(u32 first, u32 count)
{
    struct gl_state *state = gl_state_ptr();

    struct gl_pipeline *pipeline = state->pso;
    check_ptr(pipeline);

    GL_CALL( glDrawArrays(pipeline->topology, first, count) );

error:
    return;
}

void xgl_draw_indexed_impl(u32 first, u32 count)
{
    struct gl_state *state = gl_state_ptr();

    check_ptr(state->ebo);

    struct gl_pipeline *pipeline = state->pso;
    check_ptr(pipeline);

    GL_CALL( glDrawElements(pipeline->topology, count, GL_UNSIGNED_INT, (void*)(first * sizeof(GLuint))) );

error:
    return;
}
