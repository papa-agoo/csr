########################################

set(MY_NAME hello_screens)
set(MY_TARGET applet_${MY_NAME})

########################################

add_library(${MY_TARGET}
    SHARED main.c
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
    DESTINATION ${CMAKE_INSTALL_PREFIX}/applets) # FIXME applets/ path ?
