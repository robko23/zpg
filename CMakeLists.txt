cmake_minimum_required(VERSION 3.10)
project(zpg)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/**.cpp" "src/**.hpp" "src/**.h" "src/**.c")

add_definitions(-D GLFW_BUILD_X11=1 -D GLFW_BUILD_WAYLAND=0 -D _GLFW_X11)
remove_definitions(_D _GLFW_WAYLAND)

set(OpenGL_GL_PREFERENCE LEGACY)
file(GLOB LIBS "vendor/*")

find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    message("opengl found")
    message("include dir: ${OPENGL_INCLUDE_DIR}")
    message("link libraries: ${OPENGL_gl_LIBRARY}")
else (OPENGL_FOUND)
    message("opengl not found")
endif ()

add_subdirectory(${LIBS})

include_directories(vendor/glfw/include)
include_directories(vendor/glm/)

add_executable(zpg ${SOURCES})

target_link_libraries(zpg PRIVATE glfw ${OPENGL_gl_LIBRARY})