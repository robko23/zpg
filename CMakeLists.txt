cmake_minimum_required(VERSION 3.10)
project(zpg)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES
        "src/**.cpp"
        "src/**/*.cpp"
        "src/**.hpp"
        "src/**/*.hpp"
        "src/**.h"
        "src/**/*.h"
        "src/**.c"
        "src/**/*.c"
)

file(GLOB IMGUI_SOURCES "vendor/imgui/*.h" ,
        "vendor/imgui/*.cpp" ,
        "vendor/imgui/backends/imgui_impl_opengl3.h" ,
        "vendor/imgui/backends/imgui_impl_opengl3.cpp" ,
        "vendor/imgui/backends/imgui_impl_glfw.h" ,
        "vendor/imgui/backends/imgui_impl_glfw.cpp" ,
)

add_definitions(-D GLFW_BUILD_X11=1 -D GLFW_BUILD_WAYLAND=0 -D _GLFW_X11)
remove_definitions(_D _GLFW_WAYLAND)

set(OpenGL_GL_PREFERENCE LEGACY)

find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    message("opengl found")
    message("include dir: ${OPENGL_INCLUDE_DIR}")
    message("link libraries: ${OPENGL_gl_LIBRARY}")
else (OPENGL_FOUND)
    message("opengl not found")
endif ()

add_subdirectory(vendor/glm)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glew/build/cmake)
add_subdirectory(vendor/backward)
add_subdirectory(vendor/soil)

include_directories(vendor/glfw/include)
include_directories(vendor/glm/)
include_directories(vendor/glew/include)
include_directories(vendor/soil/inc/SOIL)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Wno-unknown-pragmas -Wno-sign-compare -g")
endif ()

add_executable(zpg ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(zpg PRIVATE vendor/imgui vendor/imgui/backends)

target_link_libraries(zpg PRIVATE glfw ${OPENGL_gl_LIBRARY} glew Backward::Interface bfd dl soil)
