########################################

set(MY_TARGET csr_graphics)
set(MY_ALIAS csr::graphics)

########################################

add_library(${MY_TARGET} OBJECT)
add_library(${MY_ALIAS} ALIAS ${MY_TARGET})

target_include_directories(${MY_TARGET}
    PRIVATE
        $<TARGET_PROPERTY:csr::csr,INTERFACE_INCLUDE_DIRECTORIES>
)

set_target_properties(${MY_TARGET}
    PROPERTIES POSITION_INDEPENDENT_CODE ON
)

########################################

target_sources(${MY_TARGET}
    PRIVATE
        screen.c
        screen_surface.c

        pixelbuffer.c

        xgl/xgl_driver.c
        xgl/xgl_device.c
        xgl/xgl_commands.c

        softgl/softgl_driver.c
        softgl/softgl_device.c
        softgl/softgl_commands.c
        softgl/gp_vertices.c
        softgl/gp_vertices_ia.c
        softgl/gp_primitives.c
        softgl/gp_primitives_clipper.c
        softgl/gp_primitives_rasterizer.c
        softgl/gp_primitives_rasterizer_triangle.c
)

add_subdirectory(xgl/backend)
add_subdirectory(ui)
