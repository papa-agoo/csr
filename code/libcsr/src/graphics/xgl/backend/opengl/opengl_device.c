////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

guid xgl_create_framebuffer_impl(struct vector *attachments)
{
    check_ptr(attachments);

    ////////////////////////////////////////

    struct gl_storage *storage = gl_storage_ptr();

    struct gl_framebuffer framebuffer = {0};
    GL_CALL( glCreateFramebuffers(1, &framebuffer.id) );

    u32 color_attachment_idx = 0;

    for (u32 i = 0; i < vector_size(attachments); i++)
    {
        guid *texture_id = vector_get(attachments, i);

        struct gl_texture *texture = object_pool_get(storage->textures, *texture_id);
        check_ptr(texture);

        if (framebuffer.width == 0)
        {
            framebuffer.width = texture->width;
            framebuffer.height = texture->height;
        }

        check(texture->attachment_type != GL_INVALID_ENUM,
            "texture has no valid framebuffer attachment target");

        GLenum attachment_target = texture->attachment_type;

        if (texture->attachment_type == GL_COLOR_ATTACHMENT0) {
            attachment_target += color_attachment_idx++;
        }

        GL_CALL(glNamedFramebufferTexture(framebuffer.id, attachment_target, texture->id, 0));
    }

    ////////////////////////////////////////

    GLenum fbo_status = 0;
    GL_CALL( fbo_status = glCheckNamedFramebufferStatus(framebuffer.id, GL_FRAMEBUFFER) );

    check(fbo_status == GL_FRAMEBUFFER_COMPLETE,
        "could not create framebuffer : %s", _gl_framebuffer_status_str(fbo_status));

    ////////////////////////////////////////

    return object_pool_alloc(storage->framebuffers, &framebuffer);

error:
    glDeleteFramebuffers(1, &framebuffer.id);

    return make_guid(0);
}

void xgl_destroy_framebuffer_impl(guid p_framebuffer)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_framebuffer *framebuffer = object_pool_get(storage->framebuffers, p_framebuffer);
    check_ptr(framebuffer);

    GL_CALL( glDeleteFramebuffers(1, &framebuffer->id) );

    object_pool_free(storage->framebuffers, p_framebuffer);

error:
    return;
}

void xgl_present_framebuffer_impl(guid p_framebuffer)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_framebuffer *framebuffer = object_pool_get(storage->framebuffers, p_framebuffer);
    check_ptr(framebuffer);

    GL_CALL( glBlitNamedFramebuffer(framebuffer->id, 0,
        0, 0, framebuffer->width, framebuffer->height,
        0, 0, framebuffer->width, framebuffer->height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST) );

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

static GLenum _gl_buffer_type(u32 usage_flags)
{
    // FIXME usage_flags & XGL_XXX_BIT
    if (XGL_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
        return GL_ARRAY_BUFFER;
    }

    if (XGL_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }

    if (XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        return GL_UNIFORM_BUFFER;
    }

    return GL_INVALID_ENUM;
}

static GLbitfield _gl_mapping_flags(u32 usage_flags)
{
    // need manual cpu / gpu syncing : GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    if (usage_flags & XGL_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        return GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
    }

    return 0;
}

static GLbitfield _gl_storage_flags(u32 usage_flags)
{
    // FIXME
    return GL_DYNAMIC_STORAGE_BIT;
}

guid xgl_create_buffer_impl(u32 usage_flags, size_t byte_length, void *data)
{
    ////////////////////////////////////////

    struct gl_buffer buffer = {0};
    buffer.type = _gl_buffer_type(usage_flags);
    buffer.byte_length = byte_length;
    buffer.mapping_flags = _gl_mapping_flags(usage_flags);
    buffer.storage_flags = _gl_storage_flags(usage_flags);

    GL_CALL( glCreateBuffers(1, &buffer.id) );
    GL_CALL( glNamedBufferStorage(buffer.id, buffer.byte_length,
        data, buffer.storage_flags | buffer.mapping_flags) );

    ////////////////////////////////////////

    struct gl_storage *storage = gl_storage_ptr();

    return object_pool_alloc(storage->buffers, &buffer);

error:
    return make_guid(0);
}

void xgl_destroy_buffer_impl(guid p_buffer)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_buffer *buffer = object_pool_get(storage->buffers, p_buffer);
    check_ptr(buffer);

    GL_CALL( glDeleteBuffers(1, &buffer->id) );

    object_pool_free(storage->buffers, p_buffer);

error:
    return;
}

result_e xgl_update_buffer_impl(guid p_buffer, u32 offset, u32 byte_length, void *data)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_buffer *buffer = object_pool_get(storage->buffers, p_buffer);
    check_ptr(buffer);

    GL_CALL( glNamedBufferSubData(buffer->id, offset, byte_length, data) );

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void* xgl_map_buffer_impl(guid p_buffer, u32 offset, u32 byte_length)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_buffer *buffer = object_pool_get(storage->buffers, p_buffer);
    check_ptr(buffer);

    check_expr(buffer->mapping_flags > 0);

    void *ptr = NULL;
    GL_CALL( ptr = glMapNamedBufferRange(buffer->id, offset, byte_length, buffer->mapping_flags) );

    return ptr;

error:
    return NULL;
}

void xgl_unmap_buffer_impl(guid p_buffer)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_buffer *buffer = object_pool_get(storage->buffers, p_buffer);
    check_ptr(buffer);

    GL_CALL( glUnmapNamedBuffer(buffer->id) );

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

static GLenum _sampler_addr_mode(enum xgl_sampler_addr_mode mode)
{
    switch (mode)
    {
        case XGL_SAMPLER_ADDR_REPEAT:
            return GL_REPEAT;

        case XGL_SAMPLER_ADDR_REPEAT_MIRRORED:
            return GL_MIRRORED_REPEAT;

        case XGL_SAMPLER_ADDR_CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;

        case XGL_SAMPLER_ADDR_CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;

        default:
            return GL_REPEAT;
    }
}

static GLenum _sampler_min_filter(enum xgl_texture_filter min_filter, enum xgl_mipmap_mode mip_mode)
{
    if (min_filter == XGL_TEXTURE_FILTER_NEAREST && mip_mode == XGL_MIPMAP_MODE_NONE)
        return GL_NEAREST;

    if (min_filter == XGL_TEXTURE_FILTER_NEAREST && mip_mode == XGL_MIPMAP_MODE_NEAREST)
        return GL_NEAREST_MIPMAP_NEAREST;

    if (min_filter == XGL_TEXTURE_FILTER_NEAREST && mip_mode == XGL_MIPMAP_MODE_LINEAR)
        return GL_NEAREST_MIPMAP_LINEAR;

    if (min_filter == XGL_TEXTURE_FILTER_LINEAR && mip_mode == XGL_MIPMAP_MODE_NONE)
        return GL_LINEAR;

    if (min_filter == XGL_TEXTURE_FILTER_LINEAR && mip_mode == XGL_MIPMAP_MODE_NEAREST)
        return GL_LINEAR_MIPMAP_NEAREST;

    if (min_filter == XGL_TEXTURE_FILTER_LINEAR && mip_mode == XGL_MIPMAP_MODE_LINEAR)
        return GL_LINEAR_MIPMAP_LINEAR;

    return GL_LINEAR_MIPMAP_LINEAR;
}

static GLenum _sampler_mag_filter(enum xgl_texture_filter mag_filter)
{
    return (mag_filter == XGL_TEXTURE_FILTER_LINEAR) ? GL_LINEAR : GL_NEAREST;
}

guid xgl_create_sampler_impl(struct xgl_sampler_desc *desc)
{
    check_ptr(desc);

    struct gl_sampler sampler = {0};
    GL_CALL( glCreateSamplers(1, &sampler.id) );

    ////////////////////////////////////////

    // * GL_TEXTURE_COMPARE_MODE
    // * GL_TEXTURE_COMPARE_FUNC

    // FIXME?
    if (desc->min_lod == 0) desc->min_lod = -1000.0f;
    if (desc->max_lod == 0) desc->max_lod =  1000.0f;

    GL_CALL( glSamplerParameteri(sampler.id, GL_TEXTURE_MIN_LOD, desc->min_lod) );
    GL_CALL( glSamplerParameteri(sampler.id, GL_TEXTURE_MAX_LOD, desc->max_lod) );
    GL_CALL( glSamplerParameteri(sampler.id, GL_TEXTURE_LOD_BIAS, desc->mip_lod_bias) );

    ////////////////////////////////////////

    // texture addressing
    GL_CALL( glSamplerParameteri(sampler.id, GL_TEXTURE_WRAP_S,
        _sampler_addr_mode(desc->addr_mode_u)) );

    GL_CALL( glSamplerParameteri(sampler.id, GL_TEXTURE_WRAP_T,
        _sampler_addr_mode(desc->addr_mode_v)) );

    GL_CALL( glSamplerParameteri(sampler.id, GL_TEXTURE_WRAP_R,
        _sampler_addr_mode(desc->addr_mode_w)) );

    // border color for "CLAMP_TO_BORDER" addressing
    if (desc->border_color[3] == 0) desc->border_color[3] = 1.0f;

    GL_CALL( glSamplerParameterfv(sampler.id, GL_TEXTURE_BORDER_COLOR, desc->border_color) );

    ////////////////////////////////////////

    // texture filtering
    GL_CALL( glSamplerParameteri(sampler.id, GL_TEXTURE_MIN_FILTER,
        _sampler_min_filter(desc->min_filter, desc->mipmap_mode)) );

    GL_CALL( glSamplerParameteri(sampler.id, GL_TEXTURE_MAG_FILTER,
        _sampler_mag_filter(desc->mag_filter)) );

    // anisotropy filtering
    if (desc->max_anisotropy >= 1)
    {
        f32 device_max_anisotropy = 0;
        GL_CALL( glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &device_max_anisotropy) );

        if (device_max_anisotropy < desc->max_anisotropy) {
            desc->max_anisotropy = device_max_anisotropy;
        }

        GL_CALL( glSamplerParameterf(sampler.id, GL_TEXTURE_MAX_ANISOTROPY, desc->max_anisotropy) );
    }

    ////////////////////////////////////////

    struct gl_storage *storage = gl_storage_ptr();

    return object_pool_alloc(storage->samplers, &sampler);

error:
    return make_guid(0);
}

void xgl_destroy_sampler_impl(guid p_sampler)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_sampler *sampler = object_pool_get(storage->samplers, p_sampler);
    check_ptr(sampler);

    GL_CALL( glDeleteSamplers(1, &sampler->id) );

    object_pool_free(storage->samplers, p_sampler);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

static GLenum _texture_type(enum xgl_texture_type type, u32 sample_count)
{
    switch (type)
    {
        case XGL_TEXTURE_TYPE_1D:
            return GL_TEXTURE_1D;

        case XGL_TEXTURE_TYPE_1D_ARRAY:
            return GL_TEXTURE_1D_ARRAY;

        case XGL_TEXTURE_TYPE_2D:
            return (sample_count == 1) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;

        case XGL_TEXTURE_TYPE_2D_ARRAY:
            return (sample_count == 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D_MULTISAMPLE_ARRAY;

        case XGL_TEXTURE_TYPE_3D:
            return GL_TEXTURE_3D;

        case XGL_TEXTURE_TYPE_CUBE_MAP:
            return GL_TEXTURE_CUBE_MAP;

        case XGL_TEXTURE_TYPE_CUBE_MAP_ARRAY:
            return GL_TEXTURE_CUBE_MAP_ARRAY;

        default:
            return GL_INVALID_ENUM;
    }
}

static GLenum _texture_attachment_type(u32 usage_flags)
{
    if (usage_flags & XGL_TEXTURE_USAGE_COLOR_ATTACHMENT_BIT) {
        return GL_COLOR_ATTACHMENT0;
    }

    if (usage_flags & XGL_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        return GL_DEPTH_STENCIL_ATTACHMENT;
    }

    return GL_INVALID_ENUM;
}

static GLenum _texture_pixel_format(enum xgl_texture_format format)
{
    switch (format)
    {
        case XGL_TEXTURE_FORMAT_RGBA:
            return GL_RGBA;

        case XGL_TEXTURE_FORMAT_D24_S8:
            return GL_DEPTH_STENCIL;

        default:
            return GL_INVALID_ENUM;
    }
}

static GLenum _texture_pixel_size_type(enum xgl_texture_format format)
{
    switch (format)
    {
        case XGL_TEXTURE_FORMAT_RGBA:
            return GL_UNSIGNED_BYTE;

        case XGL_TEXTURE_FORMAT_D24_S8:
            return GL_UNSIGNED_INT_24_8;


        default:
            return GL_INVALID_ENUM;
    }
}

static GLenum _texture_storage_format(enum xgl_texture_format format)
{
    switch (format)
    {
        case XGL_TEXTURE_FORMAT_RGBA:
            return GL_RGBA8;

        case XGL_TEXTURE_FORMAT_D24_S8:
            return GL_DEPTH24_STENCIL8;

        default:
            return GL_INVALID_ENUM;
    }
}

guid xgl_create_texture_impl(enum xgl_texture_type type, u32 sample_count, u32 usage_flags)
{
    ////////////////////////////////////////

    struct gl_texture texture = {0};

    texture.type = _texture_type(type, sample_count);
    texture.attachment_type = _texture_attachment_type(usage_flags);
    texture.sample_count = sample_count;

    GL_CALL( glCreateTextures(texture.type, 1, &texture.id) );

    ////////////////////////////////////////

    texture.pbo.idx_prev = 0;
    texture.pbo.idx_current = 1;
    texture.pbo.idx_next = 2;

    texture.pbo.storage_flags = GL_DYNAMIC_STORAGE_BIT;

    // need manual syncing : GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    texture.pbo.mapping_flags = GL_MAP_PERSISTENT_BIT;
    texture.pbo.mapping_flags |= (usage_flags & XGL_TEXTURE_USAGE_STREAM_READ_BIT) ? GL_MAP_READ_BIT : 0;
    texture.pbo.mapping_flags |= (usage_flags & XGL_TEXTURE_USAGE_STREAM_WRITE_BIT) ? GL_MAP_WRITE_BIT : 0;

    if (texture.pbo.mapping_flags & (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT)) {
        GL_CALL( glCreateBuffers(GL_PBO_COUNT, texture.pbo.buffer) );
    }

    ////////////////////////////////////////

    struct gl_storage *storage = gl_storage_ptr();

    return object_pool_alloc(storage->textures, &texture);

error:
    return make_guid(0);
}

void xgl_destroy_texture_impl(guid p_texture)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_texture *texture = object_pool_get(storage->textures, p_texture);
    check_ptr(texture);

    ////////////////////////////////////////

    if (texture->pbo.mapping_flags & (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT)) {
        GL_CALL( glDeleteBuffers(GL_PBO_COUNT, texture->pbo.buffer) );
    }

    ////////////////////////////////////////

    GL_CALL( glDeleteTextures(1, &texture->id) );

    object_pool_free(storage->textures, p_texture);

error:
    return;
}

result_e xgl_alloc_texture_storage_impl(guid p_texture,
    enum xgl_texture_format format, u32 mip_count, u32 width, u32 height, u32 layer_count)
{
    check_expr(format != XGL_TEXTURE_FORMAT_UNKNOWN);
    // FIXME check_expr(mip_count < driver->max_mip_level);
    check_expr(width >= 1);
    check_expr(height >= 1);
    check_expr(layer_count >= 1);

    ////////////////////////////////////////

    struct gl_storage *storage = gl_storage_ptr();

    struct gl_texture *texture = object_pool_get(storage->textures, p_texture);
    check_ptr(texture);

    texture->mip_count = mip_count;

    texture->width = width;
    texture->height = height;
    texture->layer_count = layer_count;

    texture->pixel_format = _texture_pixel_format(format);
    texture->pixel_size_type = _texture_pixel_size_type(format);
    texture->storage_format = _texture_storage_format(format);

    ////////////////////////////////////////

    switch (texture->type)
    {
        case GL_TEXTURE_1D:
        {
            GL_CALL( glTextureStorage1D(texture->id, texture->mip_count,
                texture->storage_format, texture->width) );
        }
        break;

        case GL_TEXTURE_1D_ARRAY:
        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
        {
            GL_CALL( glTextureStorage2D(texture->id, texture->mip_count,
                texture->storage_format, texture->width, texture->height) );

            if (texture->pbo.mapping_flags & (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT) && texture->type == GL_TEXTURE_2D)
            {
                texture->pbo.byte_length = texture->width * texture->height * sizeof(f32);

                for (u32 i = 0; i < GL_PBO_COUNT; i++) {
                    GL_CALL( glNamedBufferStorage(texture->pbo.buffer[i], texture->pbo.byte_length,
                        NULL, texture->pbo.mapping_flags | texture->pbo.storage_flags) );
                }
            }
        }
        break;

        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_CUBE_MAP_ARRAY:
        {
            GL_CALL( glTextureStorage3D(texture->id, texture->mip_count,
                texture->storage_format, texture->width, texture->height, texture->layer_count) );
        }
        break;

        case GL_TEXTURE_2D_MULTISAMPLE:
        {
            GL_CALL( glTextureStorage2DMultisample(texture->id, texture->sample_count,
                texture->storage_format, texture->width, texture->height, GL_TRUE) );
        }
        break;

        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        {
            GL_CALL( glTextureStorage3DMultisample(texture->id, texture->sample_count,
                texture->storage_format, texture->width, texture->height, texture->layer_count, GL_TRUE) );
        }
        break;
    }

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e xgl_update_texture_impl(guid p_texture, u32 layer, u32 width, u32 height, u8 *data)
{
    check_ptr(data);

    ////////////////////////////////////////

    struct gl_storage *storage = gl_storage_ptr();

    struct gl_texture *texture = object_pool_get(storage->textures, p_texture);
    check_ptr(texture);

    check_expr(texture->layer_count > layer);
    check_expr(texture->width == width);
    check_expr(texture->height == height);

    ////////////////////////////////////////

    if (texture->layer_count == 1)
    {
        GL_CALL( glTextureSubImage2D(texture->id, 0, 0, 0, width, height,
            texture->pixel_format, texture->pixel_size_type, data) );
    }
    else
    {
        GL_CALL( glTextureSubImage3D(texture->id, 0, 0, 0, layer, width,
            height, 1, texture->pixel_format, texture->pixel_size_type, data) );
    }

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void* xgl_map_texture_impl(guid p_texture)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_texture *texture = object_pool_get(storage->textures, p_texture);
    check_ptr(texture);

    check_expr(texture->pbo.mapping_flags > 0);
    check_expr(texture->pbo.byte_length > 0);

    ////////////////////////////////////////

    // copy texture to pbo
    if (texture->pbo.mapping_flags & GL_MAP_READ_BIT)
    {
        GLuint pbo = texture->pbo.buffer[texture->pbo.idx_next];

        GL_CALL( glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo) );

        GL_CALL( glGetTextureImage(texture->id, 0, texture->pixel_format,
            texture->pixel_size_type, texture->pbo.byte_length, 0) );

        GL_CALL( glBindBuffer(GL_PIXEL_PACK_BUFFER, 0) );
    }

    // copy pbo to texture
    if (texture->pbo.mapping_flags & GL_MAP_WRITE_BIT)
    {
        GLuint pbo = texture->pbo.buffer[texture->pbo.idx_prev];

        GL_CALL( glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo) );

        GL_CALL( glTextureSubImage2D(texture->id, 0, 0, 0, texture->width, texture->height,
            texture->pixel_format, texture->pixel_size_type, 0) );

        GL_CALL( glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0) );
    }

    // map pbo
    if (texture->pbo.ptr == NULL)
    {
        GLuint pbo = texture->pbo.buffer[texture->pbo.idx_current];

        GL_CALL( texture->pbo.ptr = glMapNamedBufferRange(pbo, 0, texture->pbo.byte_length,
            texture->pbo.mapping_flags) );
    }

    ////////////////////////////////////////

    return texture->pbo.ptr;

error:
    return NULL;
}

void xgl_unmap_texture_impl(guid p_texture)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_texture *texture = object_pool_get(storage->textures, p_texture);
    check_ptr(texture);

    check_expr(texture->pbo.ptr != NULL);

    ////////////////////////////////////////

    GLuint pbo = texture->pbo.buffer[texture->pbo.idx_current];

    // unmap pbo
    GL_CALL( glUnmapNamedBuffer(pbo) );
    texture->pbo.ptr = NULL;

    // update indices
    texture->pbo.idx_prev = (texture->pbo.idx_prev + 1) % GL_PBO_COUNT;
    texture->pbo.idx_current = (texture->pbo.idx_current + 1) % GL_PBO_COUNT;
    texture->pbo.idx_next = (texture->pbo.idx_next + 1) % GL_PBO_COUNT;

    ////////////////////////////////////////

error:
    return;
}

result_e xgl_generate_mipmaps_impl(guid p_texture)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_texture *texture = object_pool_get(storage->textures, p_texture);
    check_ptr(texture);

    GL_CALL( glGenerateTextureMipmap(texture->id) );

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void* xgl_get_texture_handle_impl(guid p_texture)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_texture *texture = object_pool_get(storage->textures, p_texture);
    check_ptr(texture);

    return (void*)(intptr_t) texture->id;

error:
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////

static GLuint _compile_shader_stage(GLenum type, string_cstr* src_ptrs, u32 src_ptr_count)
{
    GLint compile_status_ok = 0;

    // FIXME arena
    GLuint log_buf_len = 512;
    char log_str_buf[log_buf_len];

    ////////////////////////////////////////

    GLuint id = 0;
    GL_CALL( id = glCreateShader(type) );

    GL_CALL( glShaderSource(id, src_ptr_count, src_ptrs, NULL) );
    GL_CALL( glCompileShader(id) );

    ////////////////////////////////////////

    GL_CALL( glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status_ok) );

    check_expr(compile_status_ok);

    ////////////////////////////////////////

    return id;

error:

    if (!compile_status_ok)
    {
        glGetShaderInfoLog(id, log_buf_len, NULL, log_str_buf);

        clog_error("could not compile shader stage: %s", log_str_buf);
    }

    return 0;
}

static GLuint _create_shader_program(GLuint *stages, u32 stages_count)
{
    GLint link_status_ok = 0;

    GLuint log_buf_len = 512;
    char log_str_buf[log_buf_len];

    check_ptr(stages);
    check_expr(stages_count > 0);

    ////////////////////////////////////////

    GLuint pid = 0;
    GL_CALL( pid = glCreateProgram() );

    for (u32 i = 0; i < stages_count; i++) {
        GL_CALL( glAttachShader(pid, stages[i]) );
    }

    GL_CALL( glLinkProgram(pid) );

    ////////////////////////////////////////

    GL_CALL( glGetProgramiv(pid, GL_LINK_STATUS, &link_status_ok) );

    check_expr(link_status_ok);

    ////////////////////////////////////////

    return pid;

error:

    if (!link_status_ok)
    {
        glGetProgramInfoLog(pid, log_buf_len, NULL, log_str_buf);

        clog_error("could not link shader: %s", log_str_buf);
    }

    return 0;
}

guid xgl_create_shader_impl(string_cstr* vs_ptrs, u32 vs_ptr_count, string_cstr* fs_ptrs, u32 fs_ptr_count)
{
    check_ptr(vs_ptrs);
    check_expr(vs_ptr_count > 0);

    check_ptr(fs_ptrs);
    check_expr(fs_ptr_count > 0);

    ////////////////////////////////////////

    GLuint vs = _compile_shader_stage(GL_VERTEX_SHADER, vs_ptrs, vs_ptr_count);
    check(vs > 0, "could not compile vertex shader");

    GLuint fs = _compile_shader_stage(GL_FRAGMENT_SHADER, fs_ptrs, fs_ptr_count);
    check(fs > 0, "could not compile fragment shader");

    GLuint shader_stages[] = { vs, fs };

    struct gl_shader shader = {0};

    shader.id = _create_shader_program(shader_stages, COUNT_OF(shader_stages));
    check(shader.id > 0, "could not create shader program");

    glDeleteShader(vs);
    glDeleteShader(fs);

    ////////////////////////////////////////

    struct gl_storage *storage = gl_storage_ptr();

    return object_pool_alloc(storage->shaders, &shader);

error:

    if (vs > 0) glDeleteShader(vs);
    if (fs > 0) glDeleteShader(fs);

    return make_guid(0);
}

void xgl_destroy_shader_impl(guid p_shader)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_shader *shader = object_pool_get(storage->shaders, p_shader);
    check_ptr(shader);

    GL_CALL( glDeleteProgram(shader->id) );

    object_pool_free(storage->shaders, p_shader);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

static GLenum _gl_topology(enum xgl_topology type)
{
    switch (type)
    {
        case XGL_TOPOLOGY_TRIANGLE_LIST:
            return GL_TRIANGLES;

        case XGL_TOPOLOGY_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;

        case XGL_TOPOLOGY_TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;

        case XGL_TOPOLOGY_LINE_LIST:
            return GL_LINES;

        case XGL_TOPOLOGY_LINE_STRIP:
            return GL_LINE_STRIP;

        case XGL_TOPOLOGY_LINE_LOOP:
            return GL_LINE_LOOP;

        case XGL_TOPOLOGY_POINT_LIST:

        default:
            return GL_POINTS;
    }
}

CSR_INLINE GLenum _gl_compare_op(enum xgl_compare_op compare_op)
{
    switch (compare_op)
    {
        case XGL_COMPARE_OP_LESS:
            return GL_LESS;

        case XGL_COMPARE_OP_GREATER:
            return GL_GREATER;

        case XGL_COMPARE_OP_EQUAL:
            return GL_EQUAL;

        case XGL_COMPARE_OP_NOT_EQUAL:
            return GL_NOTEQUAL;

        case XGL_COMPARE_OP_LESS_EQUAL:
            return GL_LEQUAL;

        case XGL_COMPARE_OP_GREATER_EQUAL:
            return GL_GEQUAL;

        case XGL_COMPARE_OP_NEVER:
            return GL_NEVER;

        case XGL_COMPARE_OP_ALWAYS:
            return GL_ALWAYS;

        default:
            return GL_INVALID_ENUM;
    }
}

CSR_INLINE GLenum _gl_stencil_op(enum xgl_stencil_op stencil_op)
{
    switch (stencil_op)
    {
        case XGL_STENCIL_OP_KEEP:
            return GL_KEEP;

        case XGL_STENCIL_OP_ZERO:
            return GL_ZERO;

        case XGL_STENCIL_OP_REPLACE:
            return GL_REPLACE;

        case XGL_STENCIL_OP_INCREMENT_AND_CLAMP:
            return GL_INCR;

        case XGL_STENCIL_OP_DECREMENT_AND_CLAMP:
            return GL_DECR;

        case XGL_STENCIL_OP_INVERT:
            return GL_INVERT;

        case XGL_STENCIL_OP_INCREMENT_AND_WRAP:
            return GL_INCR_WRAP;

        case XGL_STENCIL_OP_DECREMENT_AND_WRAP:
            return GL_DECR_WRAP;

        default:
            return GL_INVALID_ENUM;
    }
}

CSR_INLINE GLenum _gl_front_face(enum xgl_front_face front_face)
{
    switch (front_face)
    {
        case XGL_FRONT_FACE_COUNTER_CLOCKWISE:
            return GL_CCW;

        case XGL_FRONT_FACE_CLOCKWISE:
            return GL_CW;

        default:
            return GL_INVALID_ENUM;
    }
}

CSR_INLINE GLenum _gl_cull_mode(enum xgl_cull_mode mode)
{
    switch (mode)
    {
        case XGL_CULL_MODE_BACK:
            return GL_BACK;

        case XGL_CULL_MODE_FRONT:
            return GL_FRONT;

        case XGL_CULL_MODE_FRONT_AND_BACK:
            return GL_FRONT_AND_BACK;

        default:
            return GL_INVALID_ENUM;
    }
}

CSR_INLINE GLenum _gl_blend_op(enum xgl_blend_op blend_op)
{
    switch (blend_op)
    {
        case XGL_BLEND_OP_ADD:
            return GL_FUNC_ADD;

        case XGL_BLEND_OP_SUBTRACT:
            return GL_FUNC_SUBTRACT;

        case XGL_BLEND_OP_REVERSE_SUBTRACT:
            return GL_FUNC_REVERSE_SUBTRACT;

        case XGL_BLEND_OP_MIN:
            return GL_MIN;

        case XGL_BLEND_OP_MAX:
            return GL_MAX;

        default:
            return GL_INVALID_ENUM;
    }
}

CSR_INLINE GLenum _gl_blend_factor(enum xgl_blend_factor factor)
{
    switch (factor)
    {
        case XGL_BLEND_FACTOR_ZERO:
            return GL_ZERO;

        case XGL_BLEND_FACTOR_ONE:
            return GL_ONE;

        case XGL_BLEND_FACTOR_SRC_COLOR:
            return GL_SRC_COLOR;

        case XGL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
            return GL_ONE_MINUS_SRC_COLOR;

        case XGL_BLEND_FACTOR_DST_COLOR:
            return GL_DST_COLOR;

        case XGL_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
            return GL_ONE_MINUS_DST_COLOR;

        case XGL_BLEND_FACTOR_SRC_ALPHA:
            return GL_SRC_ALPHA;

        case XGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;

        case XGL_BLEND_FACTOR_DST_ALPHA:
            return GL_DST_ALPHA;

        case XGL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
            return GL_ONE_MINUS_DST_ALPHA;

        case XGL_BLEND_FACTOR_CONSTANT_COLOR:
            return GL_CONSTANT_COLOR;

        case XGL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
            return GL_ONE_MINUS_CONSTANT_COLOR;

        case XGL_BLEND_FACTOR_CONSTANT_ALPHA:
            return GL_CONSTANT_ALPHA;

        case XGL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
            return GL_ONE_MINUS_CONSTANT_ALPHA;

        case XGL_BLEND_FACTOR_SRC_ALPHA_SATURATE:
            return GL_SRC_ALPHA_SATURATE;

        default:
            return GL_INVALID_ENUM;
    }
}

CSR_INLINE GLenum _gl_polygon_mode(enum xgl_polygon_mode mode)
{
    switch (mode)
    {
        case XGL_POLYGON_MODE_FILL:
            return GL_FILL;

        case XGL_POLYGON_MODE_LINE:
            return GL_LINE;

        case XGL_POLYGON_MODE_POINT:
            return GL_POINT;

        default:
            return GL_INVALID_ENUM;
    }
}

CSR_INLINE u32 _attrib_component_count(enum xgl_vertex_attrib type)
{
    switch (type)
    {
        case XGL_VERTEX_ATTRIB_TEX_COORD0:
        case XGL_VERTEX_ATTRIB_TEX_COORD1:
            return 2;

        default:
            return 3;
    }
}

static GLuint _create_vertex_arrays(struct vector *attrib_inputs)
{
    GLuint vao = 0;
    GL_CALL( glCreateVertexArrays(1, &vao) );

    if (!attrib_inputs) return vao;

    ////////////////////////////////////////

    u32 attrib_count = vector_size(attrib_inputs);

    for (u32 i = 0; i < attrib_count; i++)
    {
        struct xgl_vertex_attrib_input *va_input = vector_get(attrib_inputs, i);
        check_ptr(va_input);

        GL_CALL(glEnableVertexArrayAttrib(vao, va_input->location));
        GL_CALL(glVertexArrayAttribBinding(vao, va_input->location, va_input->binding));

        GLenum component_type = GL_FLOAT; // FIXME
        u32 component_count = _attrib_component_count(va_input->location);

        GL_CALL(glVertexArrayAttribFormat(vao, va_input->location,
            component_count, component_type, GL_FALSE, va_input->offset));
    }

    ////////////////////////////////////////

    return vao;

error:
    return 0;
}

guid xgl_create_pipeline_impl(struct string name, enum xgl_pipeline_type type)
{
    check_expr(type != XGL_PIPELINE_TYPE_UNKNOWN);

    struct gl_storage *storage = gl_storage_ptr();

    struct gl_pipeline pipeline = {0};
    pipeline.name = name;

    return object_pool_alloc(storage->pipelines, &pipeline);

error:
    return make_guid(0);
}

void xgl_destroy_pipeline_impl(guid p_pipeline)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline);
    check_ptr(pipeline);

    GL_CALL( glDeleteVertexArrays(1, &pipeline->vertex_arrays) );

    object_pool_free(storage->pipelines, p_pipeline);

error:
    return;
}

result_e xgl_set_pipeline_shader_state_impl(guid p_pipeline, guid p_shader)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline);
    check_ptr(pipeline);

    struct gl_shader *shader = object_pool_get(storage->shaders, p_shader);
    check_ptr(shader);

    pipeline->shader_program = shader->id;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e xgl_set_pipeline_input_state_impl(guid p_pipeline, enum xgl_topology topology, struct vector *attrib_inputs)
{
    struct gl_storage *storage = gl_storage_ptr();

    struct gl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline);
    check_ptr(pipeline);

    pipeline->topology = _gl_topology(topology);
    pipeline->vertex_arrays = _create_vertex_arrays(attrib_inputs);

    check_expr(pipeline->vertex_arrays > 0);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e xgl_set_pipeline_depth_state_impl(guid p_pipeline, struct xgl_depth_state *p_ds)
{
    check_ptr(p_ds);

    struct gl_storage *storage = gl_storage_ptr();

    struct gl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline);
    check_ptr(pipeline);

    ////////////////////////////////////////

    struct gl_depth_state *ds = &pipeline->depth_state;

    ds->enable_test = p_ds->enable_test;
    ds->enable_write = !p_ds->disable_write;
    ds->compare_op = _gl_compare_op(p_ds->compare_op);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

CSR_INLINE void _create_stencil_op_state(struct xgl_stencil_op_state *src, struct gl_stencil_op_state *dst)
{
    dst->ref_value = src->ref_value;

    dst->write_mask = src->write_mask;
    dst->compare_mask = src->compare_mask;

    dst->compare_op = _gl_compare_op(src->compare_op);

    dst->sfail_op = _gl_stencil_op(src->sfail_op);
    dst->spass_zpass_op = _gl_stencil_op(src->spass_zpass_op);
    dst->spass_zfail_op = _gl_stencil_op(src->spass_zfail_op);
}

result_e xgl_set_pipeline_stencil_state_impl(guid p_pipeline, struct xgl_stencil_state *p_ss)
{
    check_ptr(p_ss);

    struct gl_storage *storage = gl_storage_ptr();

    struct gl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline);
    check_ptr(pipeline);

    ////////////////////////////////////////

    struct gl_stencil_state *ss = &pipeline->stencil_state;

    ss->enable = p_ss->enable;

    _create_stencil_op_state(&p_ss->front, &ss->front);
    _create_stencil_op_state(&p_ss->back, &ss->back);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e xgl_set_pipeline_rasterizer_state_impl(guid p_pipeline, struct xgl_rasterizer_state *p_rs)
{
    check_ptr(p_rs);

    struct gl_storage *storage = gl_storage_ptr();

    struct gl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline);
    check_ptr(pipeline);

    ////////////////////////////////////////

    struct gl_rasterizer_state *rs = &pipeline->rasterizer_state;

    rs->discard_primitives = p_rs->discard_primitives;

    rs->enable_culling = (p_rs->cull_mode != XGL_CULL_MODE_DISABLED);

    if (rs->enable_culling)
    {
        rs->front_face = _gl_front_face(p_rs->front_face);
        check_expr(rs->front_face != GL_INVALID_ENUM);

        rs->cull_mode = _gl_cull_mode(p_rs->cull_mode);
        check_expr(rs->cull_mode != GL_INVALID_ENUM);
    }

    ////////////////////////////////////////

    rs->polygon_mode = _gl_polygon_mode(p_rs->polygon_mode);
    check_expr(rs->polygon_mode != GL_INVALID_ENUM);

    rs->point_size = p_rs->point_size;
    rs->smooth_points = p_rs->smooth_points;

    rs->line_width = p_rs->line_width;
    rs->smooth_lines = p_rs->smooth_lines;

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

result_e xgl_set_pipeline_color_blend_state_impl(guid p_pipeline, struct xgl_color_blend_state *p_bs)
{
    check_ptr(p_bs);

    struct gl_storage *storage = gl_storage_ptr();

    struct gl_pipeline *pipeline = object_pool_get(storage->pipelines, p_pipeline);
    check_ptr(pipeline);

    ////////////////////////////////////////

    struct gl_color_blend_state *bs = &pipeline->color_blend_state;

    bs->enable = p_bs->enable;

    // color / alpha blend ops
    bs->color_blend_op = _gl_blend_op(p_bs->color_blend_op);
    check_expr(bs->color_blend_op != GL_INVALID_ENUM);

    bs->alpha_blend_op = _gl_blend_op(p_bs->alpha_blend_op);
    check_expr(bs->alpha_blend_op != GL_INVALID_ENUM);

    // color blend factors
    bs->src_color_blend_factor = _gl_blend_factor(p_bs->src_color_blend_factor);
    check_expr(bs->src_color_blend_factor != GL_INVALID_ENUM);

    bs->dst_color_blend_factor = _gl_blend_factor(p_bs->dst_color_blend_factor);
    check_expr(bs->dst_color_blend_factor != GL_INVALID_ENUM);

    // alpha blend factors
    bs->src_alpha_blend_factor = _gl_blend_factor(p_bs->src_alpha_blend_factor);
    check_expr(bs->src_alpha_blend_factor != GL_INVALID_ENUM);

    bs->dst_alpha_blend_factor = _gl_blend_factor(p_bs->dst_alpha_blend_factor);
    check_expr(bs->dst_alpha_blend_factor != GL_INVALID_ENUM);

    ////////////////////////////////////////

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
