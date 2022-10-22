########################################

set(MY_TARGET csr_kernel)
set(MY_ALIAS csr::kernel)

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
        log_db.c
        event_bus.c
        mainloop.c
        mainloop_api.c

        kio.c
        kernel.c
        video_mode.c

        service/ksrv_core.c
        service/ksrv_vfs.c
        service/ksrv_video.c
        service/ksrv_audio.c
)
