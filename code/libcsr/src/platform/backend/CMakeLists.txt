if (PLATFORM_BACKEND_GLFW3)
    add_subdirectory(glfw)
else()
    message(FATAL_ERROR "platform impl. not found")
endif()
