########################################

set(MY_TARGET csr_graphics_xgl_opengl)
set(MY_ALIAS csr::graphics::xgl_impl)

########################################

find_package(Epoxy REQUIRED)

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
        opengl_driver.c
        opengl_device.c
        opengl_commands.c
)

target_link_libraries(${MY_TARGET}
    PUBLIC
        epoxy::epoxy
)
