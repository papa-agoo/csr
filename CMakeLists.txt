cmake_minimum_required( VERSION 3.12 )

project( csr C )

set(CMAKE_INSTALL_PREFIX /opt/csr)

list(APPEND
    CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake
)

option(PLATFORM_BACKEND_GLFW3 "default platform: GLFW3" ON)
option(RENDER_API_OPENGL "default render api: OpenGL" ON)

add_subdirectory(code)