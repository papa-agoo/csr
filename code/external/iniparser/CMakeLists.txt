########################################

set(MY_NAME iniparser)
set(MY_TARGET ${MY_NAME})

########################################
cmake_minimum_required( VERSION 3.12 )

project(${MY_NAME})

add_library(${MY_TARGET}
    iniparser.c
    dictionary.c
)

set_target_properties(${MY_TARGET}
    PROPERTIES POSITION_INDEPENDENT_CODE ON
)

add_library(iniparser::iniparser ALIAS ${MY_TARGET})

target_include_directories(${MY_TARGET}
    PUBLIC
        .
)
