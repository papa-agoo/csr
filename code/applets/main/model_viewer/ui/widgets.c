////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui.h>

#include <csr/core/math.h>
#include <csr/core/math/transform.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// remove the jittering of -0.000 values which are actually extremely small negative numbers.
#define F(n) f32_precision(n, 1000)

void ui_widget_mat4x4(string_cstr name, const struct mat44* matrix)
{
    f32* m = (f32*)matrix;

    igText(name);
    igText("%9.3f %9.3f %9.3f %9.3f", F(m[0]), F(m[4]),  F(m[8]), F(m[12]));
    igText("%9.3f %9.3f %9.3f %9.3f", F(m[1]), F(m[5]),  F(m[9]), F(m[13]));
    igText("%9.3f %9.3f %9.3f %9.3f", F(m[2]), F(m[6]), F(m[10]), F(m[14]));
    igText("%9.3f %9.3f %9.3f %9.3f", F(m[3]), F(m[7]), F(m[11]), F(m[15]));
}

void ui_widget_mat4x4_debug(struct mat44 matrix)
{
    static s32 idx = 0;

    igRadioButton_IntPtr("Matrix", &idx, 0);
    igRadioButton_IntPtr("Transpose", &idx, 1);
    igRadioButton_IntPtr("Inverse", &idx, 2);
    igRadioButton_IntPtr("Inverse * Matrix (Identity)", &idx, 3);

    ////////////////////////////////////////

    struct mat44 m[4] = {0};

    // M
    m[0] = matrix;

    // M_T
    m[1] = mat44_transpose(m[0]);

    // M_I
    m[2] = mat44_inverse(m[0]);

    // M_I * M
    m[3] = mat44_mult(m[2], m[0]);

    ////////////////////////////////////////

    igNewLine();
    ui_widget_mat4x4("Result", &m[idx]);
}

void ui_widget_transform(struct transform *transform, bool* show_matrix, bool invert_matrix, f64 dt)
{
    // position
    {
        if (igButton("R##position", make_ImVec2_zero())) {
            transform_set_position(transform, make_vec3_zero());
        };

        igSameLine(0, 2);

        struct vec3 position = transform_get_position(transform);

        if (igDragFloat3("Position##transform", (f32 *)(&position), 2.0f * dt, -100.0f, 100.0f, "%.3f", 1.0f)) {
            transform_set_position(transform, position);
        }
    }

    // rotation
    {
        if (igButton("R##rotation", make_ImVec2_zero())) {
            transform_set_rotation_euler(transform, make_vec3_zero());
        };

        igSameLine(0, 2);

        struct vec3 rotation = transform_get_rotation_euler(transform);

        if (igDragFloat3("Rotation##transform", (f32 *)(&rotation), 90.0f * dt, -360.0f, 360.0f, "%.3f", 1.0f)) {
            transform_set_rotation_euler(transform, rotation);
        }
    }

    // scale
    {
        if (igButton("R##scale", make_ImVec2_zero())) {
            transform_set_scale(transform, make_vec3_one());
        };

        igSameLine(0, 2);

        struct vec3 scale = transform_get_scale(transform);

        if (igDragFloat("Scale##transform", &scale.x, 2.0f * dt, -1.0f, 100.0f, "%.3f", 0)) {
            scale.z = scale.y = scale.x;
            transform_set_scale(transform, scale);
        }
    }

    // show matrix
    igNewLine();
    igCheckbox("Show Matrix", show_matrix);

    if (*show_matrix)
    {
        struct mat44 matrix = transform_get_matrix(transform);

        igNewLine();
        ui_widget_mat4x4_debug(invert_matrix ? mat44_inverse(matrix) : matrix);
    }
}
