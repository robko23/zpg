//
// Created by robko on 10/1/24.
//

#ifndef ZPG_GLWINDOW_H
#define ZPG_GLWINDOW_H

#include <optional>
#include "assertions.h"
#include "gl_info.h"
#include <iostream>

class GLWindow {
private:
    GLFWwindow* window;

    explicit GLWindow(GLFWwindow* window) : window(window) {}

    // TODO: why target != window?
#define ASSERT_SAME_WINDOW \
    DEBUG_ASSERTF(target == window, "We don't support multiple windows. We need window manager for that and that is not needed for this use case");

    void key_callback(GLFWwindow* target, int key, int scancode, int action, int mods) {
//        ASSERT_SAME_WINDOW
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        printf("key_callback [%d,%d,%d,%d] \n", key, scancode, action, mods);
    }

    void window_focus_callback(GLFWwindow* target, int focused) {
//        ASSERT_SAME_WINDOW
        printf("window_focus_callback \n");
    }

    void window_size_callback(GLFWwindow* target, int width, int height) {
//        ASSERT_SAME_WINDOW
        printf("resize %d, %d \n", width, height);
        glViewport(0, 0, width, height);
    }

    void cursor_callback(GLFWwindow* target, double x, double y) {
//        ASSERT_SAME_WINDOW
        printf("cursor_callback \n");
    }

    void button_callback(GLFWwindow* target, int button, int action, int mode) {
//        ASSERT_SAME_WINDOW
        if (action == GLFW_PRESS) printf("button_callback [%d,%d,%d]\n", button, action, mode);
    }

#undef ASSERT_SAME_WINDOW

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"

    void registerCallbacks() {
        DEBUG_ASSERT(nullptr == glfwGetWindowUserPointer(window));
        glfwSetWindowUserPointer(window, this);

        {
            auto prev =
                    glfwSetKeyCallback(window,
                                       [](GLFWwindow* target, int key, int scancode, int action,
                                          int mods) -> void {
                                           auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(
                                                   target));
                                           DEBUG_ASSERT_NOT_NULL(self);
                                           self->key_callback(target, key, scancode, action, mods);
                                       });
            DEBUG_ASSERT(nullptr == prev);
        }

        {
            auto prev = glfwSetWindowFocusCallback(window,
                                                   [](GLFWwindow* target, int focused) -> void {
                                                       auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(
                                                               target));
                                                       DEBUG_ASSERT_NOT_NULL(self);
                                                       self->window_focus_callback(target, focused);
                                                   });
            DEBUG_ASSERT(nullptr == prev);
        }

        {
            auto prev = glfwSetWindowSizeCallback(window, [](GLFWwindow* target, int width,
                                                             int height) -> void {
                auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(target));
                DEBUG_ASSERT_NOT_NULL(self);
                self->window_size_callback(target, width, height);
            });
            DEBUG_ASSERT(nullptr == prev);
        }

        {
            auto prev = glfwSetCursorPosCallback(window, [](GLFWwindow* target, double x,
                                                            double y) -> void {
                auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(target));
                DEBUG_ASSERT_NOT_NULL(self);
                self->cursor_callback(target, x, y);
            });
            DEBUG_ASSERT(nullptr == prev)
        }

        {
            auto prev = glfwSetMouseButtonCallback(window,
                                                   [](GLFWwindow* target, int button, int action,
                                                      int mode) -> void {
                                                       auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(
                                                               target));
                                                       DEBUG_ASSERT_NOT_NULL(self);
                                                       self->button_callback(target, button, action,
                                                                             mode);
                                                   });
            DEBUG_ASSERT(nullptr == prev);
        }
    }

#pragma clang diagnostic pop

public:
    GLWindow(GLWindow &&other) noexcept: window(other.window) {
        other.window = nullptr;
    }

    GLWindow(GLWindow &other) noexcept: window(other.window) {
        other.window = nullptr;
    }

    static std::optional<GLWindow> create(const char* title) {
        GLFWwindow* window = glfwCreateWindow(800, 600, title, nullptr, nullptr);
        if (nullptr == window) {
            std::cerr << "ERROR: could not create GLFW window. are you in glfw context?"
                      << std::endl;
            return {};
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        glewExperimental = GL_TRUE;
        GLenum init_status = glewInit();
        // HUH?
//        if (GLEW_OK != init_status) {
//            auto err = glewGetErrorString(init_status);
//            std::cerr << "ERROR: failed to init glew: " << init_status << " " << err << std::endl;
//            glfwDestroyWindow(window);
//            glfwMakeContextCurrent(nullptr);
//            return {};
//        }

        DEBUG_ASSERT_NOT_NULL(window);
        auto instance = GLWindow(window);
        instance.registerCallbacks();

        glfwMakeContextCurrent(nullptr);

        return instance;
    }

    void inContext(const std::function<void()> &func) {
        if (nullptr != glfwGetCurrentContext()) {
            throw std::logic_error("Double enter to glfw window context");
        }
        glfwMakeContextCurrent(window);

        glfwSwapInterval(1);
        glEnable(GL_DEPTH_TEST);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        func();
        glfwMakeContextCurrent(nullptr);
    }

    void swapBuffers() const noexcept {
        if (window) {
            glfwSwapBuffers(window);
#ifdef DEBUG_ASSERTIONS
            int err = glfwGetError(nullptr);
            DEBUG_ASSERT(err == GLFW_NO_ERROR);
#endif
        }
    }

    [[nodiscard]] inline bool shouldClose() const noexcept {
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
