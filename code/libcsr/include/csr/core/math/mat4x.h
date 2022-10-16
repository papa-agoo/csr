////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vec3.h"
#include "vec4.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// mat44_{set,get}_{row,column,diagonal}()

// mat44_get_translation_matrix()
// mat44_get_rotation_matrix()
// mat44_get_scale_matrix()
// mat44_invert()

// mat44_decompose()
// mat44_lookat() (non inverted)
// mat44_determinant()

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

CSR_INLINE struct mat44 mat44_add(struct mat44 a, struct mat44 b)
{
    f32* a_ptr = (f32*) &a;
    f32* b_ptr = (f32*) &b;

    for (u32 i = 0; i < 16; i++)
        a_ptr[i] += b_ptr[i];
}

CSR_INLINE struct mat44 mat44_sub(struct mat44 a, struct mat44 b)
{
    f32* a_ptr = (f32*) &a;
    f32* b_ptr = (f32*) &b;

    for (u32 i = 0; i < 16; i++)
        a_ptr[i] -= b_ptr[i];
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

CSR_INLINE f32 mat44_determinant(struct mat44 m)
{
    // FIXME
    return 0;
}

CSR_INLINE struct mat44 mat44_get_translation_matrix(struct mat44 m)
{
    struct mat44 mt = mat44_identity();

    // FIXME
    return mt;
}

CSR_INLINE struct mat44 mat44_get_rotation_matrix(struct mat44 m)
{
    struct mat44 mr = mat44_identity();

    // FIXME
    return mr;
}

CSR_INLINE struct mat44 mat44_get_scale_matrix(struct mat44 m)
{
    struct mat44 ms = mat44_identity();

    // FIXME
    return ms;
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

CSR_INLINE struct mat44 mat44_invert(struct mat44 m)
{
    // FIXME
    return m;
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

    return mat44_mult(ry, mat44_mult(rz, rx));
}

CSR_INLINE struct mat44 mat44_rotate_zxy(struct vec3 axes)
{
    // FIXME
    return mat44_identity();
}

CSR_INLINE struct mat44 mat44_compose(struct vec3 position, struct vec3 rotation, struct vec3 scale)
{
    struct mat44 ms = mat44_scale(scale);
    struct mat44 mr = mat44_rotate(rotation);
    struct mat44 mt = mat44_translate(position);

    return mat44_mult(mt, mat44_mult(mr, ms));
}

CSR_INLINE void mat44_decompose(struct mat44 m, struct vec3 *position, struct vec3 *rotation, struct vec3 *scale)
{
    check_ptr(position);
    check_ptr(rotation);
    check_ptr(scale);

    // FIXME

    *position = make_vec3(m.wx, m.wy, m.wz);
    *rotation = make_vec3_zero();
    *scale = make_vec3_one();

error:
    return;
}

CSR_INLINE struct mat44 mat44_lookat(struct vec3 eye, struct vec3 target, struct vec3 up)
{
    struct mat44 m = mat44_identity();

    m.wx = eye.x;
    m.wy = eye.y;
    m.wz = eye.z;

    // FIXME
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
