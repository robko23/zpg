//
// Created by robko on 10/1/24.
//

#ifndef ZPG_GLWINDOW_H
#define ZPG_GLWINDOW_H

#include <optional>
#include <GL/glew.h>
#include <GL/gl.h>
#include "assertions.h"
#include <iostream>

class GLWindow {
private:
    GLFWwindow* window;
    explicit GLWindow(GLFWwindow* window): window(window) {}

public:
    GLWindow(GLWindow&& other) noexcept : window(other.window) {
        other.window = nullptr;
    }
    GLWindow(GLWindow& other) noexcept: window(other.window) {
        other.window = nullptr;
    }

    static std::optional<GLWindow> create(const char* title) {
        GLFWwindow* window = glfwCreateWindow(800, 600, "ZPG", nullptr, nullptr);
        if (nullptr == window) {
            glfwTerminate();
            std::cerr << "ERROR: could not create GLFW window. are you in glfw context?" << std::endl;
            return {};
        }
        glfwMakeContextCurrent(window);

        glewExperimental = GL_TRUE;
        GLenum init_status = glewInit();
        if (GLEW_OK != init_status) {
            std::cerr << "ERROR: failed to init glew" << std::endl;
            glfwDestroyWindow(window);
            glfwMakeContextCurrent(nullptr);
            return {};
        }

        glfwMakeContextCurrent(nullptr);
        DEBUG_ASSERT_NOT_NULL(window);
        return GLWindow(window);
    }

    void inContext(const std::function<void()>& func) {
        if (nullptr != glfwGetCurrentContext()) {
            throw std::logic_error("Double enter to glfw window context");
        }
        glfwMakeContextCurrent(window);

        glfwSwapInterval(1);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        func();
        glfwMakeContextCurrent(nullptr);
    }

    void swapBuffers() const noexcept{
        if (window) {
            glfwSwapBuffers(window);
#ifdef DEBUG_ASSERTIONS
            int err = glfwGetError(nullptr);
            DEBUG_ASSERT(err == GLFW_NO_ERROR);
#endif
        }
    }

    [[nodiscard]] inline bool shouldClose() const noexcept{
        if (window) {
            return glfwWindowShouldClose(window);
        } else {
            return true;
        }
    }

    ~GLWindow() {
        if (window) {
            glfwDestroyWindow(window);
        }
    }
};

#endif //ZPG_GLWINDOW_H
