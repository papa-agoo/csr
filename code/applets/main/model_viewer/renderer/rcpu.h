////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// #include <csr/graphics/softgl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// assumptions
//  - vertex format (supported attribs: position, color, uv, normal)
//  - ...

// render pass setup
//  - prepare color + depth buffers
//  - set state
//      - rasterizer (viewport, )
//      - ...
//  - set resources
//      - matrices
//      - lights
//      - ...

// per mesh setup
//  - build draw cmd from mesh_view
//      - geometry desc
//          - vertex layout (for input assembler)
//      - material desc

// ...

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rcpu_conf
{
    void *foo;
};

struct rcpu_create_info
{
    struct screen *screen;
};

struct rcpu
{
    struct screen *screen;

    struct rcpu_conf conf;
};

struct rcpu* rcpu_create();
void rcpu_destroy(struct rcpu* rcpu);
