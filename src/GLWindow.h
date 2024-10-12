//
// Created by robko on 10/1/24.
//

#ifndef ZPG_GLWINDOW_H
#define ZPG_GLWINDOW_H

#include <optional>
#include "assertions.h"
#include "gl_info.h"
#include <iostream>
#include <memory>

class GLWindow {
private:
    GLFWwindow* window;
    // Normal printable keys defined in glfw3.h from GLFW_KEY_SPACE up to GLFW_KEY_GRAVE_ACCENT
    uint64_t printableKeyStatus;

    // the difference between first and last function keys as defined by GLFW is 96, so we
    // cannot fit it in one 64-bit number, and we have to slice it up.
    // keys GLFW_KEY_ESCAPE up to GLFW_KEY_KP_0
    uint64_t functionKeys0;
    // keys GLFW_KEY_KP_1 up to GLFW_KEY_MENU (that is 28 used bits, maybe we can fit modifier keys
    // here as well? tbd
    uint64_t functionKeys1;

    double lastTime;
    double delta;

    inline void setKey(int key) {
        if (key <= GLFW_KEY_GRAVE_ACCENT && key >= GLFW_KEY_SPACE) {
            printableKeyStatus |= 1 << (GLFW_KEY_GRAVE_ACCENT - key);
        } else if (key <= GLFW_KEY_KP_0 && key >= GLFW_KEY_ESCAPE) {
            functionKeys0 |= 1 << (GLFW_KEY_KP_0 - key);
        } else if (key <= GLFW_KEY_MENU && key >= GLFW_KEY_KP_1) {
            functionKeys1 |= 1 << (GLFW_KEY_MENU - key);
        }
    }

    inline void clearKey(int key) {
        if (key <= GLFW_KEY_GRAVE_ACCENT && key >= GLFW_KEY_SPACE) {
            printableKeyStatus &= ~(1 << (GLFW_KEY_GRAVE_ACCENT - key));
        } else if (key <= GLFW_KEY_KP_0 && key >= GLFW_KEY_ESCAPE) {
            functionKeys0 &= ~(1 << (GLFW_KEY_KP_0 - key));
        } else if (key <= GLFW_KEY_MENU && key >= GLFW_KEY_KP_1) {
            functionKeys1 &= ~(1 << (GLFW_KEY_MENU - key));
        }
    }

    inline bool getKey(int key) const {
        if (key <= GLFW_KEY_GRAVE_ACCENT && key >= GLFW_KEY_SPACE) {
            return (printableKeyStatus & (1 << (GLFW_KEY_GRAVE_ACCENT - key))) != 0;
        } else if (key <= GLFW_KEY_KP_0 && key >= GLFW_KEY_ESCAPE) {
            return (functionKeys0 & (1 << (GLFW_KEY_KP_0 - key))) != 0;
        } else if (key <= GLFW_KEY_MENU && key >= GLFW_KEY_KP_1) {
            return (functionKeys1 & 1 << (GLFW_KEY_MENU - key)) != 0;
        }
        return false;
    }

    explicit GLWindow(GLFWwindow* window) : window(window), printableKeyStatus(0), lastTime(0),
                                            functionKeys0(0), functionKeys1(0) {
    }

    void key_callback(GLFWwindow* target, int key, int scancode, int action, int mods) {
        if (GLFW_PRESS == action) {
            setKey(key);
        } else if (GLFW_RELEASE == action) {
            clearKey(key);
        }
//        printf("key_callback [%d,%d,%d,%d] \n", key, scancode, action, mods);
    }

    void window_focus_callback(GLFWwindow* target, int focused) {
//        printf("window_focus_callback \n");
    }

    void window_size_callback(GLFWwindow* target, int width, int height) {
        printf("resize %d, %d \n", width, height);
        glViewport(0, 0, width, height);
    }

    void cursor_callback(GLFWwindow* target, double x, double y) {
//        printf("cursor_callback \n");
    }

    void button_callback(GLFWwindow* target, int button, int action, int mode) {
//        if (action == GLFW_PRESS) printf("button_callback [%d,%d,%d]\n", button, action, mode);
    }


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

    GLWindow(GLWindow &other) : window(other.window), printableKeyStatus(other.printableKeyStatus),
                                lastTime(other.lastTime), functionKeys0(other.functionKeys0),
                                functionKeys1(other.functionKeys1), delta(other.delta) {
        other.window = nullptr;
        other.printableKeyStatus = 0;
        other.lastTime = 0;
        other.functionKeys0 = 0;
        other.functionKeys1 = 0;
        other.delta = 0;
    }

    GLWindow(GLWindow &&other) noexcept: window(other.window),
                                         printableKeyStatus(other.printableKeyStatus),
                                         lastTime(other.lastTime),
                                         functionKeys0(other.functionKeys0),
                                         functionKeys1(other.functionKeys1), delta(other.delta) {
        other.window = nullptr;
        other.printableKeyStatus = 0;
        other.lastTime = 0;
        other.functionKeys0 = 0;
        other.functionKeys1 = 0;
        other.delta = 0;
    }

    [[nodiscard]] inline bool isPressed(int key) const noexcept {
        return getKey(key);
    }

    bool isPressedAndClear(int key) {
        bool val = getKey(key);
        clearKey(key);
        return val;
    }

    [[nodiscard]] double getDelta() const {
        return delta;
    }

    static std::optional<std::unique_ptr<GLWindow>> create(const char* title) {
        GLFWwindow* window = glfwCreateWindow(800, 600, title, nullptr, nullptr);
        if (nullptr == window) {
            std::cerr << "ERROR: could not create GLFW window. are you in glfw context?"
                      << std::endl;
            return {};
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        glewExperimental = GL_TRUE;
        glewInit();

        DEBUG_ASSERT_NOT_NULL(window);
        auto instance = std::make_unique<GLWindow>(GLWindow(window));
        instance->registerCallbacks();

        glfwMakeContextCurrent(nullptr);

        return std::move(instance);
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

    void endFrame() noexcept {
        if (window) {
            glfwSwapBuffers(window);
#ifdef DEBUG_ASSERTIONS
            int err = glfwGetError(nullptr);
            DEBUG_ASSERT(err == GLFW_NO_ERROR);
#endif

            double now = glfwGetTime();
            delta = now - lastTime;
            lastTime = now;
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
