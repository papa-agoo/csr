////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rcpu_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

vertex_shader(vs_vertex_color)
{
    // resource bindings
    layout_buffer(FRAME, shader_data_frame, frame_data);
    layout_buffer(OBJECT, shader_data_object, object_data);

    // inputs
    vs_in(POSITION, vec3, a_position);
    vs_in(COLOR, vec3, a_color);

    // outputs
    vs_out(vec4, v_color);

    // main
    {
        *v_color = make_vec4_3_1(*a_color, 1.0f);

        struct mat44 mvp = object_data->mat_mvp;

        // FIXME use object mvp where possible
        if (!object_data->use_object_mvp) {
            mvp = mat44_mult(mat44_mult(frame_data->mat_projection, frame_data->mat_view), object_data->mat_model);
        }

        vs_return( mat44_mult_vec4(mvp, make_vec4_3_1(*a_position, 1.0f)) );
    }
}

fragment_shader(fs_vertex_color)
{
    // inputs
    fs_in(0, vec4, v_color);

    // main
    fs_return( *v_color );
}
