########################################

set(MY_TARGET csr_applet)
set(MY_ALIAS csr::applet)

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
        applet_plugin.c
        applet_db.c
)
