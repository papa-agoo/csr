////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gp.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GEOMETRY PIPELINE FLOW
//
// - gp_reset_state
//
// - gp_assemble_primitives
//      - validate_state
//      - process_vertices
//          << LOOP >>
//              - create_vertex()
//              - update primitives list
//                  - a) sequential_list (points, lines, triangles)
//
//
// - gp_assemble_primitives_indexed
//      - validate_state_indexed
//      - process_vertices
//          << LOOP ebo >>
//              - locate_vertex()
//                  - not found: create_vertex()
//              - update primitives list
//                  - a) sequential_list (points, lines, triangles)
//                  - b) line_strip
//                  - c) line_loop
//                  - d) triangle_strip
//                  - e) triangle_fan
// 
////////////////////////////////////////////////////////////////////////////////////////////////////

static void _update_vertex_attrib_ptrs(u32 index)
{
    struct softgl_pipeline *pso = softgl_pso_ptr();
    struct softgl_graphics_pipeline *gp = softgl_gp_ptr();

    for (u32 i = 0; i < vector_size(pso->attrib_inputs); i++)
    {
        // lookup vertex attrib
        struct softgl_vertex_attrib_input *va = vector_get(pso->attrib_inputs, i);

        // lookup vertex buffer for this vertex attrib
        struct softgl_vertex_buffer *vb = softgl_binding_ptr().vertex_buffers[va->binding];

        // calc vertex attrib address and write it to the lookup table
        struct vertex_attrib_input_ptrs *va_ptrs = &gp->ia.va_ptrs;
        va_ptrs->va_map[va->attrib] = (vb->stride * index) + (vb->buffer.data + va->offset);
    }
}

static struct softgl_vertex* _create_vertex(u32 index)
{
    struct softgl_pipeline *pso = softgl_pso_ptr();
    struct softgl_graphics_pipeline *gp = softgl_gp_ptr();

    // init vertex
    ////////////////////////////////////////

    u32 ia_vertex_id = gp->ia.num_processed_vertices++;
    u32 vc_vertex_id = gp->vc.vertex_count++;

    struct softgl_vertex* vertex = &gp->vc.vertices[vc_vertex_id];

    // set system inputs
    vertex->vertex_id = ia_vertex_id;
    vertex->primitive_id = 0;

    // reset attrib outputs
    vertex->attribs_out.count = 0;


    // process vertex
    ////////////////////////////////////////

    // update attrib pointers
    _update_vertex_attrib_ptrs(index);

    // exec vertex shader
    pso->shader->vertex_shader_ptr(&gp->ia.va_ptrs, vertex);

    return vertex;
}

#define _lookup_vertex_index(i) ((index_buffer) ? ((u32*)index_buffer->buffer.data)[i] : i)

void gp_process_vertices(struct softgl_index_buffer *index_buffer)
{
    struct softgl_graphics_pipeline *gp = softgl_gp_ptr();

    ////////////////////////////////////////

    // reset vertex cache
    struct vertex_cache* vc = &gp->vc;

    vc->index_count = 0;
    vc->vertex_count = 0;

    // reset primitive cache
    struct primitive_cache* pc = &gp->pc;

    pc->primitive_count = 0;
    pc->vertex_count = 0;

    ////////////////////////////////////////

    struct input_assembler* ia = &gp->ia;

    for (u32 i = ia->vs.start; i < ia->vs.stop; i++)
    {
        struct softgl_vertex* vertex = _create_vertex(_lookup_vertex_index(i));

        pc->vertices[pc->vertex_count++] = vertex;
        pc->primitive_count++;
    }
}
