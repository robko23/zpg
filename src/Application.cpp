//
// Created by robko on 9/30/24.
//

#include <iostream>
#include <csignal>
#include "Application.h"

Application* Application::instance = nullptr;

void Application::print_info() { // NOLINT(*-convert-member-functions-to-static)
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    std::cout << "Using GLFW " << major << "." << minor << "." << revision << std::endl;
}

bool Application::init() {
    if (nullptr != Application::instance) {
        std::cout << "WARN: double initialization of application app" << std::endl;
        return false;
    }

    glfwSetErrorCallback([](int error, const char* description) -> void {
        std::cerr << "ERROR: in glfw: error code: " << error << ", error description: "
                  << description << std::endl;
    });

    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW3" << std::endl;
        return false;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "ZPG", nullptr, nullptr);
    if (nullptr == window) {
        glfwTerminate();
        std::cerr << "ERROR: could not create GLFW window" << std::endl;
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    GLenum init_status = glewInit();
    if (GLEW_OK != init_status) {
        std::cerr << "ERROR: failed to init glew" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    assert(nullptr == Application::instance);
    auto app = new Application(window);
    Application::instance = app;
    return true;
}

Application::Application(GLFWwindow* window) : window(window) {
    std::cout << "initialization successful" << std::endl;
    this->print_info();
}

Application* Application::get_instance() {
    if (nullptr == Application::instance) {
        std::cerr << "FATAL: Called Application::get_instance before Application::init" << std::endl;
        raise(SIGABRT);
    }
    return Application::instance;
}

void Application::tick(const std::function<void()> &render) {
    // clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render();

    // update other events like input handling
    glfwPollEvents();
    // put the stuff we’ve been drawing onto the display
    glfwSwapBuffers(window);
}

Application::~Application() {
    assert(nullptr != window);
    glfwDestroyWindow(window);

    glfwTerminate();
}
