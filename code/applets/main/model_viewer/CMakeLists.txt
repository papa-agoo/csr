########################################

set(MY_NAME model_viewer)
set(MY_TARGET applet_${MY_NAME})

########################################

add_library(${MY_TARGET}
    SHARED
        main.c
        model_viewer.c
        model_viewer_input.c

        ui/widgets.c
        ui/register.c
        ui/view_model.c
        ui/view_camera.c
        ui/view_rsx.c

        scene.c
        scene/model.c
        scene/model_import_obj.c
        scene/model_import_gltf.c
        scene/camera.c

        rsx.c
        rsx_commands.c
        rsx_resources.c
        rsx/rgpu.c
        rsx/rgpu_cache.c
        rsx/rcpu.c
        rsx/rcpu_cache.c
        rsx/rcpu_cache_shaders.c
)

target_include_directories(${MY_TARGET}
    PRIVATE
        .
)

target_link_libraries(${MY_TARGET}
    PRIVATE
        csr::csr
)

set_target_properties(${MY_TARGET}
    PROPERTIES PREFIX ""
)

set_target_properties(${MY_TARGET}
    PROPERTIES OUTPUT_NAME ${MY_NAME}
)

install(TARGETS ${MY_TARGET}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/applets)
