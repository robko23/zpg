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
#include "GLWindow.h"


class Application {
    // Application holds an invariant: is instance exists, glfw is initialized, glew is
    // initialized and there is a glfw window created,
private:
    static inline Application* instance = nullptr;

    explicit Application(GLWindow window);

    void tick(const std::function<void()> &render);

public:
    GLWindow window;
    Application(Application const &) = delete;

    void operator=(Application const &) = delete;

    static Application* get_instance();

    static bool init();

    void run(const std::function<void()> &render) {
//        window.inContext([this, &render]() -> void {
            while (!window.shouldClose()) {
                this->tick(render);
            }
//        });
    }
};



#endif //ZPG_APPLICATION_H
