########################################

set(MY_TARGET csr_graphics_xgl)
set(MY_ALIAS csr::graphics::xgl)

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
        xgl_driver.c
        xgl_device.c
        xgl_commands.c
)

add_subdirectory(backend)
