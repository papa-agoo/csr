////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vec3.h"
#include "vec4.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mat44
{
    // column major format
    // xx yx zx wx
    // xy yy zy wy
    // xz yz zz wz
    // xw yw zw ww

    f32 xx, xy, xz, xw;
    f32 yx, yy, yz, yw;
    f32 zx, zy, zz, zw;
    f32 wx, wy, wz, ww;
};

#define make_mat44(...) \
    make_compound_literal(struct mat44, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////

CSR_INLINE struct mat44 mat44_identity()
{
    //  1  0  0  0
    //  0  1  0  0
    //  0  0  1  0
    //  0  0  0  1

    return make_mat44(.xx = 1.0f, .yy = 1.0f, .zz = 1.0f, .ww = 1.0f);
}

CSR_INLINE bool mat44_equals(struct mat44 a, struct mat44 b)
{
    f32* a_ptr = (f32*) &a;
    f32* b_ptr = (f32*) &b;

    for (u32 i = 0; i < 16; i++) {
        if (! f32_equal(a_ptr[i], b_ptr[i])) return false;
    }

    return true;
}

CSR_INLINE bool mat44_is_identity(struct mat44 m)
{
    return mat44_equals(mat44_identity(), m);
}

CSR_INLINE struct mat44 mat44_add(struct mat44 a, struct mat44 b)
{
    f32* a_ptr = (f32*) &a;
    f32* b_ptr = (f32*) &b;

    for (u32 i = 0; i < 16; i++) {
        a_ptr[i] += b_ptr[i];
    }

    return a;
}

CSR_INLINE struct mat44 mat44_sub(struct mat44 a, struct mat44 b)
{
    f32* a_ptr = (f32*) &a;
    f32* b_ptr = (f32*) &b;

    for (u32 i = 0; i < 16; i++) {
        a_ptr[i] -= b_ptr[i];
    }

    return a;
}

CSR_INLINE struct mat44 mat44_mult(struct mat44 a, struct mat44 b)
{
    return make_mat44(
        // x column
        .xx = (a.xx * b.xx) + (a.yx * b.xy) + (a.zx * b.xz) + (a.wx * b.xw),
        .xy = (a.xy * b.xx) + (a.yy * b.xy) + (a.zy * b.xz) + (a.wy * b.xw),
        .xz = (a.xz * b.xx) + (a.yz * b.xy) + (a.zz * b.xz) + (a.wz * b.xw),
        .xw = (a.xw * b.xx) + (a.yw * b.xy) + (a.zw * b.xz) + (a.ww * b.xw),

        // y column
        .yx = (a.xx * b.yx) + (a.yx * b.yy) + (a.zx * b.yz) + (a.wx * b.yw),
        .yy = (a.xy * b.yx) + (a.yy * b.yy) + (a.zy * b.yz) + (a.wy * b.yw),
        .yz = (a.xz * b.yx) + (a.yz * b.yy) + (a.zz * b.yz) + (a.wz * b.yw),
        .yw = (a.xw * b.yx) + (a.yw * b.yy) + (a.zw * b.yz) + (a.ww * b.yw),

        // z column
        .zx = (a.xx * b.zx) + (a.yx * b.zy) + (a.zx * b.zz) + (a.wx * b.zw),
        .zy = (a.xy * b.zx) + (a.yy * b.zy) + (a.zy * b.zz) + (a.wy * b.zw),
        .zz = (a.xz * b.zx) + (a.yz * b.zy) + (a.zz * b.zz) + (a.wz * b.zw),
        .zw = (a.xw * b.zx) + (a.yw * b.zy) + (a.zw * b.zz) + (a.ww * b.zw),

        // w column
        .wx = (a.xx * b.wx) + (a.yx * b.wy) + (a.zx * b.wz) + (a.wx * b.ww),
        .wy = (a.xy * b.wx) + (a.yy * b.wy) + (a.zy * b.wz) + (a.wy * b.ww),
        .wz = (a.xz * b.wx) + (a.yz * b.wy) + (a.zz * b.wz) + (a.wz * b.ww),
        .ww = (a.xw * b.wx) + (a.yw * b.wy) + (a.zw * b.wz) + (a.ww * b.ww),
    );
}

CSR_INLINE struct mat44 mat44_mult_scalar(struct mat44 m, f32 scalar)
{
    f32* m_ptr = (f32*) &m;

    for (u32 i = 0; i < 16; i++) {
        m_ptr[i] *= scalar;
    }

    return m;
}

CSR_INLINE struct vec3 mat44_mult_vec3(struct mat44 m, struct vec3 v)
{
    return make_vec3(
        .x = (m.xx * v.x) + (m.yx * v.y) + (m.zx * v.z) + m.wx,
        .y = (m.xy * v.x) + (m.yy * v.y) + (m.zy * v.z) + m.wy,
        .z = (m.xz * v.x) + (m.yz * v.y) + (m.zz * v.z) + m.wz
    );
}

CSR_INLINE struct vec4 mat44_mult_vec4(struct mat44 m, struct vec4 v)
{
    return make_vec4(
        .x = (m.xx * v.x) + (m.yx * v.y) + (m.zx * v.z) + (m.wx * v.w),
        .y = (m.xy * v.x) + (m.yy * v.y) + (m.zy * v.z) + (m.wy * v.w),
        .z = (m.xz * v.x) + (m.yz * v.y) + (m.zz * v.z) + (m.wz * v.w),
        .w = (m.xw * v.x) + (m.yw * v.y) + (m.zw * v.z) + (m.ww * v.w)
    );
}

CSR_INLINE struct mat44 mat44_transpose(struct mat44 m)
{
    return make_mat44(
        .xx = m.xx,
        .xy = m.yx,
        .xz = m.zx,
        .xw = m.wx,

        .yx = m.xy,
        .yy = m.yy,
        .yz = m.zy,
        .yw = m.wy,

        .zx = m.xz,
        .zy = m.yz,
        .zz = m.zz,
        .zw = m.wz,

        .wx = m.xw,
        .wy = m.yw,
        .wz = m.zw,
        .ww = m.ww,
    );
}

CSR_INLINE f32 mat44_determinant(struct mat44 m)
{
    return (
        m.xx * (m.yy * (m.zz * m.ww - m.zw * m.wz) + m.yz * (m.zw * m.wy - m.zy * m.ww) + m.yw * (m.zy * m.wz - m.zz * m.wy)) -
        m.xy * (m.yx * (m.zz * m.ww - m.zw * m.wz) + m.yz * (m.zw * m.wx - m.zx * m.ww) + m.yw * (m.zx * m.wz - m.zz * m.wx)) +
        m.xz * (m.yx * (m.zy * m.ww - m.zw * m.wy) + m.yy * (m.zw * m.wx - m.zx * m.ww) + m.yw * (m.zx * m.wy - m.zy * m.wx)) -
        m.xw * (m.yx * (m.zy * m.wz - m.zz * m.wy) + m.yy * (m.zz * m.wx - m.zx * m.wz) + m.yz * (m.zx * m.wy - m.zy * m.wx))
    );
}

CSR_INLINE struct mat44 mat44_inverse(struct mat44 m)
{
    f32 determinant = mat44_determinant(m);

    check_expr(determinant != 0);

    f32 determinant_inv = 1.0 / determinant;

    return make_mat44(
        .xx = determinant_inv * ( m.yy * m.zz * m.ww - m.yy * m.zw * m.wz - m.zy * m.yz * m.ww + m.zy * m.yw * m.wz + m.wy * m.yz * m.zw - m.wy * m.yw * m.zz),
        .yx = determinant_inv * (-m.yx * m.zz * m.ww + m.yx * m.zw * m.wz + m.zx * m.yz * m.ww - m.zx * m.yw * m.wz - m.wx * m.yz * m.zw + m.wx * m.yw * m.zz),
        .zx = determinant_inv * ( m.yx * m.zy * m.ww - m.yx * m.zw * m.wy - m.zx * m.yy * m.ww + m.zx * m.yw * m.wy + m.wx * m.yy * m.zw - m.wx * m.yw * m.zy),
        .wx = determinant_inv * (-m.yx * m.zy * m.wz + m.yx * m.zz * m.wy + m.zx * m.yy * m.wz - m.zx * m.yz * m.wy - m.wx * m.yy * m.zz + m.wx * m.yz * m.zy),

        .xy = determinant_inv * (-m.xy * m.zz * m.ww + m.xy * m.zw * m.wz + m.zy * m.xz * m.ww - m.zy * m.xw * m.wz - m.wy * m.xz * m.zw + m.wy * m.xw * m.zz),
        .yy = determinant_inv * ( m.xx * m.zz * m.ww - m.xx * m.zw * m.wz - m.zx * m.xz * m.ww + m.zx * m.xw * m.wz + m.wx * m.xz * m.zw - m.wx * m.xw * m.zz),
        .zy = determinant_inv * (-m.xx * m.zy * m.ww + m.xx * m.zw * m.wy + m.zx * m.xy * m.ww - m.zx * m.xw * m.wy - m.wx * m.xy * m.zw + m.wx * m.xw * m.zy),
        .wy = determinant_inv * ( m.xx * m.zy * m.wz - m.xx * m.zz * m.wy - m.zx * m.xy * m.wz + m.zx * m.xz * m.wy + m.wx * m.xy * m.zz - m.wx * m.xz * m.zy),

        .xz = determinant_inv * ( m.xy * m.yz * m.ww - m.xy * m.yw * m.wz - m.yy * m.xz * m.ww + m.yy * m.xw * m.wz + m.wy * m.xz * m.yw - m.wy * m.xw * m.yz),
        .yz = determinant_inv * (-m.xx * m.yz * m.ww + m.xx * m.yw * m.wz + m.yx * m.xz * m.ww - m.yx * m.xw * m.wz - m.wx * m.xz * m.yw + m.wx * m.xw * m.yz),
        .zz = determinant_inv * ( m.xx * m.yy * m.ww - m.xx * m.yw * m.wy - m.yx * m.xy * m.ww + m.yx * m.xw * m.wy + m.wx * m.xy * m.yw - m.wx * m.xw * m.yy),
        .wz = determinant_inv * (-m.xx * m.yy * m.wz + m.xx * m.yz * m.wy + m.yx * m.xy * m.wz - m.yx * m.xz * m.wy - m.wx * m.xy * m.yz + m.wx * m.xz * m.yy),

        .xw = determinant_inv * (-m.xy * m.yz * m.zw + m.xy * m.yw * m.zz + m.yy * m.xz * m.zw - m.yy * m.xw * m.zz - m.zy * m.xz * m.yw + m.zy * m.xw * m.yz),
        .yw = determinant_inv * ( m.xx * m.yz * m.zw - m.xx * m.yw * m.zz - m.yx * m.xz * m.zw + m.yx * m.xw * m.zz + m.zx * m.xz * m.yw - m.zx * m.xw * m.yz),
        .zw = determinant_inv * (-m.xx * m.yy * m.zw + m.xx * m.yw * m.zy + m.yx * m.xy * m.zw - m.yx * m.xw * m.zy - m.zx * m.xy * m.yw + m.zx * m.xw * m.yy),
        .ww = determinant_inv * ( m.xx * m.yy * m.zz - m.xx * m.yz * m.zy - m.yx * m.xy * m.zz + m.yx * m.xz * m.zy + m.zx * m.xy * m.yz - m.zx * m.xz * m.yy),
    );

error:
    return mat44_identity();
}

CSR_INLINE struct mat44 mat44_translate(struct vec3 t)
{
    //  1  0  0 Tx
    //  0  1  0 Ty
    //  0  0  1 Tz
    //  0  0  0  1

    struct mat44 m = mat44_identity();

    m.wx = t.x;
    m.wy = t.y;
    m.wz = t.z;

    return m;
}

CSR_INLINE struct vec3 mat44_get_translation(struct mat44 m)
{
    return make_vec3(
        .x = m.wx,
        .y = m.wy,
        .z = m.wz
    );
}

CSR_INLINE struct mat44 mat44_get_translation_matrix(struct mat44 m)
{
    struct vec3 t = mat44_get_translation(m);

    struct mat44 mt = mat44_identity();

    mt.wx = t.x;
    mt.wy = t.y;
    mt.wz = t.z;

    return mt;
}

CSR_INLINE struct mat44 mat44_scale(struct vec3 s)
{
    // Sx  0  0  0
    //  0 Sy  0  0
    //  0  0 Sz  0
    //  0  0  0  1

    struct mat44 m = mat44_identity();

    m.xx = s.x;
    m.yy = s.y;
    m.zz = s.z;

    return m;
}

CSR_INLINE struct vec3 mat44_get_scale(struct mat44 m)
{
    return make_vec3(
        .x = vec3_length( make_vec3(m.xx, m.xy, m.xz) ),
        .y = vec3_length( make_vec3(m.yx, m.yy, m.yz) ),
        .z = vec3_length( make_vec3(m.zx, m.zy, m.zz) )
    );
}

CSR_INLINE struct mat44 mat44_get_scale_matrix(struct mat44 m)
{
    struct vec3 s = mat44_get_scale(m);

    struct mat44 ms = mat44_identity();

    ms.xx = s.x;
    ms.yy = s.y;
    ms.zz = s.z;

    return ms;
}

CSR_INLINE struct mat44 mat44_rotate_x(f32 degrees)
{
    // 1  0   0 0
    // 0 cy -sz 0
    // 0 sy  cz 0
    // 0  0   0 1

    f32 cos = cosf(DEG2RAD(degrees));
    f32 sin = sinf(DEG2RAD(degrees));

    struct mat44 m = mat44_identity();

    m.yy =  cos;
    m.yz =  sin;

    m.zy = -sin;
    m.zz =  cos;

    return m;
}

CSR_INLINE struct mat44 mat44_rotate_y(f32 degrees)
{
    //  cx 0 sz 0
    //   0 1  0 0
    // -sx 0 cz 0
    //   0 0  0 1

    f32 cos = cosf(DEG2RAD(degrees));
    f32 sin = sinf(DEG2RAD(degrees));

    struct mat44 m = mat44_identity();

    m.xx =  cos;
    m.xz = -sin;

    m.zx =  sin;
    m.zz =  cos;

    return m;
}

CSR_INLINE struct mat44 mat44_rotate_z(f32 degrees)
{
    // cx -sy 0 0
    // sx  cy 0 0
    //  0   0 1 0
    //  0   0 0 1

    f32 cos = cosf(DEG2RAD(degrees));
    f32 sin = sinf(DEG2RAD(degrees));

    struct mat44 m = mat44_identity();

    m.xx =  cos;
    m.xy =  sin;

    m.yx = -sin;
    m.yy =  cos;

    return m;
}

CSR_INLINE struct mat44 mat44_rotate(struct vec3 axes)
{
    struct mat44 rx = mat44_rotate_x(axes.x);
    struct mat44 ry = mat44_rotate_y(axes.y);
    struct mat44 rz = mat44_rotate_z(axes.z);

    // rotation order: XZY
    return mat44_mult(ry, mat44_mult(rz, rx));
}

CSR_INLINE struct mat44 mat44_get_rotation_matrix(struct mat44 m)
{
    struct vec3 s = mat44_get_scale(m);

    struct mat44 mr = mat44_identity();

    f32 sx_inv = 1.0f / s.x;
    mr.xx = m.xx * sx_inv;
    mr.xy = m.xy * sx_inv;
    mr.xz = m.xz * sx_inv;

    f32 ys_inv = 1.0f / s.y;
    mr.yx = m.yx * ys_inv;
    mr.yy = m.yy * ys_inv;
    mr.yz = m.yz * ys_inv;

    f32 zs_inv = 1.0f / s.z;
    mr.zx = m.zx * zs_inv;
    mr.zy = m.zy * zs_inv;
    mr.zz = m.zz * zs_inv;

    return mr;
}

CSR_INLINE struct vec3 mat44_get_rotation(struct mat44 m)
{
    struct mat44 mr = mat44_get_rotation_matrix(m);

    // rotation order: XZY
    struct vec3 r = {0};

    if (mr.xy < 1)
    {
        if (mr.xy > -1)
        {
            r.x = atan2f(mr.zy, mr.yy);
            r.y = atan2f(mr.xz, mr.xx);
            r.z = asinf(-mr.xy);
        }
        else {
            r.x = -atan2f(-mr.zx, mr.zz);
            r.z = M_PI * 0.5;
        }
    }
    else {
        r.x = atan2f(-mr.zx, mr.zz);
        r.z = -M_PI * 0.5;
    }

    // FIXME rewrite factorization so that no inverting is needed
    return vec3_scale(make_vec3(RAD2DEG(r.x), RAD2DEG(r.y), RAD2DEG(r.z)), -1);
}

CSR_INLINE struct mat44 mat44_compose(struct vec3 position, struct vec3 rotation, struct vec3 scale)
{
    struct mat44 ms = mat44_scale(scale);
    struct mat44 mr = mat44_rotate(rotation);
    struct mat44 mt = mat44_translate(position);

    // transform order: S, R, T
    return mat44_mult(mt, mat44_mult(mr, ms));
}

CSR_INLINE void mat44_decompose(struct mat44 m, struct vec3 *position, struct vec3 *rotation, struct vec3 *scale)
{
    check_ptr(position);
    check_ptr(rotation);
    check_ptr(scale);

    // FIXME handle numerical instabilities
    *position = mat44_get_translation(m);
    *scale = mat44_get_scale(m);
    *rotation = mat44_get_rotation(m);

error:
    return;
}

CSR_INLINE struct mat44 mat44_lookat(struct vec3 eye, struct vec3 target, struct vec3 up)
{
    // see http://www.songho.ca/opengl/gl_camera.html

    ////////////////////////////////////////

    struct vec3 forward = vec3_normalize( vec3_sub(eye, target) );
    struct vec3 right = vec3_normalize( vec3_cross(up, forward) );
    struct vec3 up_real = vec3_cross(forward, right);

    ////////////////////////////////////////

    //  Rx Ux Fx Tx
    //  Ry Uy Fy Ty
    //  Rz Uz Fz Tz
    //  0  0  0  1

    struct mat44 m = mat44_identity();

    m.xx = right.x;
    m.xy = right.y;
    m.xz = right.z;

    m.yx = up_real.x;
    m.yy = up_real.y;
    m.yz = up_real.z;

    m.zx = forward.x;
    m.zy = forward.y;
    m.zz = forward.z;

    m.wx = eye.x;
    m.wy = eye.y;
    m.wz = eye.z;

    return m;
}

CSR_INLINE struct mat44 mat44_lookat_inv(struct vec3 eye, struct vec3 target, struct vec3 up)
{
    // see http://www.songho.ca/opengl/gl_camera.html

    ////////////////////////////////////////

    struct vec3 forward = vec3_normalize( vec3_sub(eye, target) );
    struct vec3 right = vec3_normalize( vec3_cross(up, forward) );
    struct vec3 up_real = vec3_cross(forward, right);

    ////////////////////////////////////////

    struct mat44 m = mat44_identity();

    m.xx = right.x;
    m.xy = up_real.x;
    m.xz = forward.x;

    m.yx = right.y;
    m.yy = up_real.y;
    m.yz = forward.y;

    m.zx = right.z;
    m.zy = up_real.z;
    m.zz = forward.z;

    m.wx = - vec3_dot(right, eye);
    m.wy = - vec3_dot(up_real, eye);
    m.wz = - vec3_dot(forward, eye);

    return m;
}

CSR_INLINE struct mat44 mat44_ortho(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    // see http://www.songho.ca/opengl/gl_projectionmatrix.html

    struct mat44 m = mat44_identity();

    m.xx = 2 / (r - l);
    m.wx = - (r + l) / (r - l);

    m.yy = 2 / (t - b);
    m.wy = - (t + b) / (t - b);

    m.zz = -2 / (f - n);
    m.wz = - (f + n) / (f - n);

    return m;
}

CSR_INLINE struct mat44 mat44_frustum(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    // see http://www.songho.ca/opengl/gl_projectionmatrix.html

    struct mat44 m = mat44_identity();

    m.xx = (2 * n) / (r - l);
    m.zx = (r + l) / (r - l);

    m.yy = (2 * n) / (t - b);
    m.zy = (t + b) / (t - b);

    m.zz = - (f + n) / (f - n);
    m.zw = -1;

    m.wz = - (2 * f * n) / (f - n);
    m.ww = 0;

    return m;
}

CSR_INLINE struct mat44 mat44_fovy(f32 fovy, f32 aspect, f32 n, f32 f)
{
    f32 t = n * tanf(DEG2RAD(fovy/2.0));
    f32 b = -t;

    f32 r = t * aspect;
    f32 l = -r;

    return mat44_frustum(l, r, b, t, n, f);
}
