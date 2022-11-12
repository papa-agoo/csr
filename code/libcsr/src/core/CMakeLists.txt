########################################

set(MY_TARGET csr_core)
set(MY_ALIAS csr::core)

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
        list.c
        vector.c
        hashmap.c
        object_pool.c
        config.c
        path.c
        file_io.c
        clock.c

        base/guid.c
        math/transform.c
)
