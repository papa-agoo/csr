########################################

set(MY_TARGET csr_graphics_ui)
set(MY_ALIAS csr::graphics::ui)

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
        cimgui_priv.c

        ui.c
        ui_themes.c

        theme/imgui_dark.c
        theme/csr_graphite.c
)

target_link_libraries(${MY_TARGET}
    PUBLIC
        cimgui::cimgui
)

add_subdirectory( imgui_backends )
