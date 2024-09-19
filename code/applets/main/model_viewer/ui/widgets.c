////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui.h>

#include <csr/core/math.h>
#include <csr/core/math/transform.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
void ui_widget_mat4x4(string_cstr name, const struct mat44* matrix)
{
    f32* m = (f32*)matrix;

    igText(name);
    igText("%7.3f %7.3f %7.3f %7.3f ", m[0], m[4], m[8], m[12]);
    igText("%7.3f %7.3f %7.3f %7.3f ", m[1], m[5], m[9], m[13]);
    igText("%7.3f %7.3f %7.3f %7.3f ", m[2], m[6], m[10], m[14]);
    igText("%7.3f %7.3f %7.3f %7.3f ", m[3], m[7], m[11], m[15]);
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
    m[2] = mat44_invert(m[0]);

    // M_I * M
    m[3] = mat44_mult(m[2], m[0]);

    ////////////////////////////////////////

    igNewLine();
    ui_widget_mat4x4("Result", &m[idx]);
}

void ui_widget_transform(struct transform *transform, bool* show_matrix, f64 dt)
{
    // position
    {
        if (igButton("R##position", make_ImVec2_zero())) {
            transform_set_position(transform, make_vec3_zero());
        };

        igSameLine(0, 2);

        struct vec3 position = transform_get_position(transform);

        if (igDragFloat3("Position##model", (f32 *)(&position), 2.0f * dt, -100.0f, 100.0f, "%.3f", 1.0f)) {
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

        if (igDragFloat3("Rotation##model", (f32 *)(&rotation), 90.0f * dt, -360.0f, 360.0f, "%.3f", 1.0f)) {
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

        if (igDragFloat("Scale##model", &scale.x, 2.0f * dt, -1.0f, 100.0f, "%.3f", 0)) {
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
        ui_widget_mat4x4_debug(matrix);
    }
}
