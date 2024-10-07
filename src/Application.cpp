//
// Created by robko on 9/30/24.
//

#include <iostream>
#include <csignal>
#include <utility>
#include "Application.h"
#include "GLWindow.h"

bool Application::init() {
    if (nullptr != Application::instance) {
        std::cout << "WARN: double initialization of application app" << std::endl;
        return false;
    }
    auto window = GLWindow::create("ZPG");

    assert(nullptr == Application::instance);
    auto app = new Application(window.value());
    Application::instance = app;
    return true;
}

Application::Application(GLWindow window) : window(std::move(window)) {
    std::cout << "initialization successful" << std::endl;
}

Application* Application::get_instance() {
    if (nullptr == Application::instance) {
        std::cerr << "FATAL: Called Application::get_instance before Application::init"
                  << std::endl;
        raise(SIGABRT);
    }
    return Application::instance;
}

void Application::tick(const std::function<void()> &render) {
    // clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render();

    // update other events like input handling
    // TODO: where to put this?
    glfwPollEvents();

    // put the stuff we’ve been drawing onto the display
    window.swapBuffers();
}