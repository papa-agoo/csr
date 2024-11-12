////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

#include <epoxy/gl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

CSR_INLINE string_cstr _gl_type_str(GLenum type)
{
    switch (type)
    {
        case GL_DOUBLE:
            return "GL_DOUBLE";

        case GL_FLOAT:
            return "GL_FLOAT";

        case GL_INT:
            return "GL_INT";

        case GL_UNSIGNED_INT:
            return "GL_UNSIGNED_INT";

        case GL_FLOAT_VEC2:
            return "GL_FLOAT_VEC2";

        case GL_FLOAT_VEC3:
            return "GL_FLOAT_VEC3";

        case GL_FLOAT_VEC4:
            return "GL_FLOAT_VEC4";

        case GL_FLOAT_MAT3:
            return "GL_FLOAT_MAT3";

        case GL_FLOAT_MAT4:
            return "GL_FLOAT_MAT4";

        case GL_SAMPLER_CUBE:
            return "GL_SAMPLER_CUBE";

        case GL_SAMPLER_1D:
            return "GL_SAMPLER_1D";

        case GL_SAMPLER_2D:
            return "GL_SAMPLER_2D";

        case GL_SAMPLER_3D:
            return "GL_SAMPLER_3D";

        default:
            return "CSR_UNKNOWN_GL_TYPE";
    }
}

CSR_INLINE string_cstr _gl_framebuffer_status_str(GLenum fbo_status)
{
    switch (fbo_status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return "GL_FRAMEBUFFER_COMPLETE";

        case GL_FRAMEBUFFER_UNDEFINED:
            return "GL_FRAMEBUFFER_UNDEFINED";

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";

        case GL_FRAMEBUFFER_UNSUPPORTED:
            return "GL_FRAMEBUFFER_UNSUPPORTED";

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";

        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";

        default:
            return "CSR_UNKNOWN_GL_FRAMEBUFFER_STATUS";
    }
}

CSR_INLINE string_cstr _gl_error_str(GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";

        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";

        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";

        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";

        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";

        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";

        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";

        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";

        default:
            return "CSR_UNKNOWN_GL_ERROR";
    }
}

CSR_INLINE string_cstr _gl_error_source_cstr(GLenum source)
{
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            return "API";

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW SYSTEM";

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER COMPILER";

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD PARTY";

        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";

        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";

        default:
            return "CSR_UNKNOWN_GL_ERROR_SOURCE";
    }
}

CSR_INLINE string_cstr _gl_error_type_cstr(GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";

        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";

        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";

        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";

        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";

        default:
            return "CSR_UNKNOWN_GL_ERROR_TYPE";
    }
}

CSR_INLINE string_cstr _gl_error_severity_cstr(GLenum severity)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";

        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";

        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";

        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";

        default:
            return "CSR_UNKNOWN_GL_ERROR_SEVERITY";
    }
}

////////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
    #define GL_CALL(cmd) do { \
            cmd; \
            check( R_SUCCESS(_gl_check_error(#cmd, __FILE__, __LINE__)), "GL_CALL( %s ) failed", _mstring(cmd) ); \
        } while (0)
#else
    #define GL_CALL(cmd) cmd
#endif

CSR_INLINE result_e _gl_check_error(string_cstr cmd, string_cstr fname, s32 line)
{
    result_e result = RC_SUCCESS;

    GLenum error = GL_NO_ERROR;

    while ((error = glGetError() && error != GL_NO_ERROR))
    {
        clog_trace("!!! %08x (%s), at %s:%i - for %s\n", error, _gl_error_str(error), fname, line, cmd);

        result = RC_FAILURE;
    }

    return result;
}

static void _gl_message_callback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
    clog_trace("%s, %s, %s, %d : %s", _gl_error_source_cstr(source),
        _gl_error_type_cstr(type), _gl_error_severity_cstr(severity), id, message);
}

static void opengl_init_debug_callbacks()
{
#ifndef NDEBUG

    // https://www.khronos.org/opengl/wiki/Debug_Output
    s32 flags;
    GL_CALL( glGetIntegerv(GL_CONTEXT_FLAGS, &flags) );

    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        clog_trace("Enabling OpenGL Debug Output ...");

        GL_CALL( glEnable(GL_DEBUG_OUTPUT) );
        GL_CALL( glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS) );

        GL_CALL( glDebugMessageCallback(_gl_message_callback, NULL) );

        GL_CALL( glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
            GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE) );
    }

#endif

error:
    return;
}
