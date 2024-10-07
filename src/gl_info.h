//
// Created by robko on 10/1/24.
//

#ifndef ZPG_GL_INFO_H
#define ZPG_GL_INFO_H

#include <GL/glew.h>
#include "assertions.h"
#include <iostream>
#include <GLFW/glfw3.h>

inline void print_gl_info() {
    auto gl_version = glGetString(GL_VERSION);
    DEBUG_ASSERT_NOT_NULL(gl_version);
    std::cout << "OpenGL Version: " << gl_version << std::endl;

    auto glew_version = glewGetString(GLEW_VERSION);
    DEBUG_ASSERT_NOT_NULL(glew_version);
    std::cout << "Using GLEW " << glew_version << std::endl;

    auto gl_vendor = glGetString(GL_VENDOR);
    DEBUG_ASSERT_NOT_NULL(gl_vendor);
    std::cout << "Vendor: " << gl_vendor << std::endl;

    auto gl_renderer = glGetString(GL_RENDERER);
    DEBUG_ASSERT_NOT_NULL(gl_renderer);
    std::cout << "Renderer: " << gl_renderer << std::endl;

    auto glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
    DEBUG_ASSERT_NOT_NULL(glsl);
    std::cout << "GLSL: " << glsl << std::endl;

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    std::cout << "Using GLFW " << major << "." << minor << "." << revision << std::endl;

    std::flush(std::cout);
}

#endif //ZPG_GL_INFO_H
