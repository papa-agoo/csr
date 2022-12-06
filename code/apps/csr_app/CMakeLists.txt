########################################

set(MY_NAME csr_app)
set(MY_TARGET ${MY_NAME})

########################################

add_executable(${MY_TARGET}
    main.c
    application.c

    frontend/workspace.c

    frontend/ctx_applet.c
    frontend/ctx_application.c

    frontend/view_settings.c
    frontend/view_settings/page_kernel.c
    frontend/view_settings/page_frontend.c

    frontend/view_applet_db.c
    frontend/view_log_messages.c
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

install(
    DIRECTORY
        ${CMAKE_SOURCE_DIR}/resources
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}
)
