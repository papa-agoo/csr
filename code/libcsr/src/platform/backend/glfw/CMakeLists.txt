########################################

set(MY_TARGET csr_platform_glfw)
set(MY_ALIAS csr::platform::impl)

########################################

find_package(GLFW REQUIRED)
find_package(Epoxy REQUIRED)

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
        platform_glfw3.c
        hid_mappings.c
)

target_link_libraries(${MY_TARGET}
    PUBLIC
        GLFW3::GLFW3
        epoxy::epoxy
)
