########################################

set(MY_NAME csr)
set(MY_TARGET ${MY_NAME})

########################################

add_library(${MY_TARGET} SHARED)
add_library(csr::csr ALIAS ${MY_TARGET})

target_include_directories(${MY_TARGET}
    PUBLIC
        include
    PUBLIC
        $<TARGET_PROPERTY:stb::image,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:bstrlib::bstrlib,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:tinyobjloader-c,INTERFACE_INCLUDE_DIRECTORIES>
)

add_subdirectory(src/core)
add_subdirectory(src/platform)
add_subdirectory(src/kernel)
add_subdirectory(src/graphics)
add_subdirectory(src/applet)

target_link_libraries(${MY_TARGET}
    PUBLIC
        m
    PUBLIC
        bstrlib::bstrlib
        iniparser::iniparser
    PUBLIC
        csr::core
        csr::platform
        csr::platform::impl
        csr::kernel
        csr::graphics
        csr::graphics::xgl_impl
        csr::graphics::ui
        csr::graphics::ui_platform_impl
        csr::graphics::ui_renderer_impl
        csr::applet
)

install(TARGETS ${MY_TARGET}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
