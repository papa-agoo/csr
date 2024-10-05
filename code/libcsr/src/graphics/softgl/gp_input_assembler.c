////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static u32 _adjust_vertex_count(enum softgl_topology topology, u32 vertex_count)
{
    switch (topology)
    {
        case SOFTGL_TOPOLOGY_POINT_LIST:
            return vertex_count;

        case SOFTGL_TOPOLOGY_LINE_LIST:
            return (vertex_count < 2) ? 0 : vertex_count - (vertex_count % 2);

        case SOFTGL_TOPOLOGY_LINE_STRIP:
        case SOFTGL_TOPOLOGY_LINE_LOOP:
            return (vertex_count < 2) ? 0: vertex_count;

        case SOFTGL_TOPOLOGY_TRIANGLE_LIST:
            return (vertex_count < 3) ? 0 : vertex_count - (vertex_count % 3);

        case SOFTGL_TOPOLOGY_TRIANGLE_STRIP:
        case SOFTGL_TOPOLOGY_TRIANGLE_FAN:
            return (vertex_count < 3) ? 0: vertex_count;
    }
}

// static u32 _calc_primitive_count(enum softgl_topology topology, u32 vertex_count)
// {
//     if (vertex_count == 0) return 0;

//     switch (topology)
//     {
//         case SOFTGL_TOPOLOGY_POINT_LIST:
//             return vertex_count;

//         case SOFTGL_TOPOLOGY_LINE_LIST:
//             return vertex_count / 2;

//         case SOFTGL_TOPOLOGY_LINE_STRIP:
//             return vertex_count - 1;

//         case SOFTGL_TOPOLOGY_LINE_LOOP:
//             return vertex_count;

//         case SOFTGL_TOPOLOGY_TRIANGLE_LIST:
//             return vertex_count / 3;

//         case SOFTGL_TOPOLOGY_TRIANGLE_STRIP:
//         case SOFTGL_TOPOLOGY_TRIANGLE_FAN:
//             return vertex_count - 2;
//     }
// }

static u32 _calc_vertices_per_seek(enum softgl_topology topology, u32 cache_size)
{
    // vertices per primitive
    u32 verts_per_prim = 0;

    switch (topology)
    {
        case SOFTGL_TOPOLOGY_POINT_LIST:
            verts_per_prim = 1;
        break;

        case SOFTGL_TOPOLOGY_LINE_LIST:
        case SOFTGL_TOPOLOGY_LINE_STRIP:
        case SOFTGL_TOPOLOGY_LINE_LOOP:
            verts_per_prim = 2;
        break;

        case SOFTGL_TOPOLOGY_TRIANGLE_LIST:
        case SOFTGL_TOPOLOGY_TRIANGLE_STRIP:
        case SOFTGL_TOPOLOGY_TRIANGLE_FAN:
            verts_per_prim = 3;
        break;
    }

    ////////////////////////////////////////

    // max vertices per seek
    u32 max_verts_per_seek = (cache_size - (cache_size % verts_per_prim));

    // max primitives per seek
    u32 max_prims_per_seek = max_verts_per_seek / verts_per_prim;

    ////////////////////////////////////////

    // seek size
    switch (topology)
    {
        case SOFTGL_TOPOLOGY_POINT_LIST:
        case SOFTGL_TOPOLOGY_LINE_LIST:
        case SOFTGL_TOPOLOGY_TRIANGLE_LIST:
            return max_verts_per_seek;
        break;

        case SOFTGL_TOPOLOGY_LINE_STRIP:
        case SOFTGL_TOPOLOGY_LINE_LOOP:
            return max_prims_per_seek + 1;
        break;

        case SOFTGL_TOPOLOGY_TRIANGLE_STRIP:
        case SOFTGL_TOPOLOGY_TRIANGLE_FAN:
            return max_prims_per_seek + 2;
        break;
    }
}

static void _stream_setup(struct vertex_stream* vs, enum softgl_topology topology, u32 first, u32 vertex_count)
{
    // IA vertex count for the draw call
    u32 ia_vertex_count = _adjust_vertex_count(topology, vertex_count);

    if (ia_vertex_count == 0) return;

    ////////////////////////////////////////

    // IA max vertex count for one batch
    u32 ia_verts_per_seek = _calc_vertices_per_seek(topology, VERTEX_CACHE_SIZE);

    if (ia_verts_per_seek == 0) return;

    ////////////////////////////////////////

    vs->head = first;
    vs->tail = first + ia_vertex_count;

    vs->start = 0;
    vs->stop = 0;

    vs->seek_size = ia_verts_per_seek;
}

void gp_ia_setup(struct input_assembler* ia, enum softgl_topology topology, u32 first, u32 vertex_count)
{
    ia->topology = topology;
    ia->num_processed_vertices = 0;

    ia->va_ptrs = (struct vertex_attrib_input_ptrs){0};
    ia->vs = (struct vertex_stream){0};

    _stream_setup(&ia->vs, topology, first, vertex_count);
}

bool gp_ia_seek_stream(struct vertex_stream* vs)
{
    if (vs->stop == vs->tail) {
        return false;
    }

    vs->start = vs->stop;
    vs->stop = vs->start + vs->seek_size;

    if (vs->stop > vs->tail) {
        vs->stop = vs->tail;
    }

    return true;
}
