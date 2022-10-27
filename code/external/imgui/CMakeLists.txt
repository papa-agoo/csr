########################################

set(MY_NAME cimgui)
set(MY_TARGET ${MY_NAME})

########################################

cmake_minimum_required( VERSION 3.12 )

project(${MY_NAME})

add_library(${MY_TARGET} SHARED
    ./cimgui/cimgui.cpp
    ./cimgui/imgui/imgui.cpp
    ./cimgui/imgui/imgui_draw.cpp
    ./cimgui/imgui/imgui_demo.cpp
    ./cimgui/imgui/imgui_widgets.cpp
    ./cimgui/imgui/imgui_tables.cpp
)

if (RENDER_API_OPENGL)
    target_sources(${MY_TARGET}
        PRIVATE
            ./cimgui/imgui/backends/imgui_impl_opengl3.cpp
    )
endif ()

if (PLATFORM_BACKEND_GLFW3)
    target_sources(${MY_TARGET}
        PRIVATE
            ./cimgui/imgui/backends/imgui_impl_glfw.cpp
    )
endif ()


add_library(cimgui::cimgui ALIAS ${MY_TARGET})

target_compile_definitions(${MY_TARGET} PUBLIC
    "IMGUI_IMPL_API=extern \"C\" "
)

target_include_directories(${MY_TARGET}
    PUBLIC
        ./cimgui
    PRIVATE
        ./cimgui/imgui
)

install(TARGETS ${MY_TARGET}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
