//
// Created by robko on 10/1/24.
//

#ifndef ZPG_GLFWCONTEXT_H
#define ZPG_GLFWCONTEXT_H

#include "GLFW/glfw3.h"
#include <functional>
#include <iostream>
#include <ostream>
#include <stdexcept>

class GLFWcontext {

private:
    static inline bool in_context = false;

public:
    explicit GLFWcontext() = delete;

    static void inContext(const std::function<void()>& func) {
        if (in_context) {
            throw std::logic_error("Double initialization of glfw");
        }
        std::cout << "Initializing glfw" << std::endl;
        if (!glfwInit()) {
            std::cerr << "ERROR: could not start GLFW3" << std::endl;
            return;
        }
        std::cout << "Initialization of glfw successful" << std::endl;

        glfwSetErrorCallback([](int error, const char* description) -> void {
            std::cerr << "ERROR: in glfw: error code: " << error << ", error description: "
                      << description << std::endl;
        });

        std::cout << "Invoking func" << std::endl;
        func();

        std::cout << "Terminating glfw context" << std::endl;
        glfwTerminate();
        glfwSetErrorCallback(nullptr);
        in_context = false;
    }
};

#endif //ZPG_GLFWCONTEXT_H
