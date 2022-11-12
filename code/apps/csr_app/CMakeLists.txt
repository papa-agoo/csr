########################################

set(MY_NAME csr_app)
set(MY_TARGET ${MY_NAME})

########################################

add_executable(${MY_TARGET}
    main.c

    frontend/workspace.c
    frontend/ctx_application.c

    frontend/view_console.c
    frontend/view_log_messages.c
    frontend/view_applet_db.c
    frontend/view_application_settings.c
)

target_include_directories(${MY_TARGET}
    PRIVATE
        .
)

target_link_libraries(${MY_TARGET}
    PRIVATE
        csr::csr
)

install(
    TARGETS
        ${MY_TARGET}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/bin
)
