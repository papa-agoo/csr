########################################

set(MY_NAME test_file_io)
set(MY_TARGET ${MY_NAME})

########################################

add_executable(${MY_TARGET}
    main.c
)

target_link_libraries(${MY_TARGET}
    PRIVATE
        csr::csr
)

install(
    TARGETS
        ${MY_TARGET}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/tests
)
