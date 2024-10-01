//
// Created by robko on 9/30/24.
//

#ifndef ZPG_APPLICATION_H
#define ZPG_APPLICATION_H

#include <GL/glew.h>
#include <cstdio>
#include <GLFW/glfw3.h>
#include <cassert>
#include <functional>
#include <optional>
#include <memory>


class Application {
    // Application holds an invariant: is instance exists, glfw is initialized, glew is
    // initialized and there is a glfw window created,
private:
    static Application* instance;
    GLFWwindow* window;

    explicit Application(GLFWwindow* window);

    void tick(const std::function<void()> &render);

public:
    Application(Application const &) = delete;

    void operator=(Application const &) = delete;

    void print_info();

    static Application* get_instance();

    static bool init();

    void run(const std::function<void()> &render) {
        while (!glfwWindowShouldClose(window)) {
            this->tick(render);
        }
    }

    ~Application();
};



#endif //ZPG_APPLICATION_H
