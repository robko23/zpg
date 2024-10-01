//
// Created by robko on 9/30/24.
//

#ifndef ZPG_APPLICATION_H
#define ZPG_APPLICATION_H

#include <GL/glew.h>
#include <cstdio>
#include <GLFW/glfw3.h>


class GLFWContext {
private:

    friend class Application;

    ~GLFWContext() {

    }
};

class Application {
    // Application holds an invariant: is instance exists, glfw is initialized, glew is
    // initialized and there is a glfw window created,
private:
    static Application* instance;
    GLFWwindow* window;

    explicit Application(GLFWwindow* window) : window(window) {
        printf("initialization successfull");
        this->printInfo();
    };


public:
    void printInfo();

    inline static Application* get_instance() {
        if (nullptr == Application::instance) {
            return Application::init();
        }
        return Application::instance;
    }

    static Application* init() {
        glfwSetErrorCallback([](int error, const char* description) -> void {
            fprintf(stderr, "ERROR: in glfw: error code: %d, error description: %s", error,
                    description);
        });

        if (nullptr != Application::instance) {
            printf("WARN: double initialization of application app\n");
            return Application::instance;
        }
        if (!glfwInit()) {
            fprintf(stderr, "ERROR: could not start GLFW3\n");
            return nullptr;
        }

        GLFWwindow* window = glfwCreateWindow(800, 600, "ZPG", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            fprintf(stderr, "ERROR: could not create GLFW window\n");
            return nullptr;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        // start GLEW extension handler
        glewExperimental = GL_TRUE;
        GLenum init_status = glewInit();
        if (init_status != GLEW_OK) {
            fprintf(stderr, "ERROR: failed to init glew\n");
            glfwTerminate();
            return nullptr;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        auto app = new Application(window);
        Application::instance = app;
        return app;
    }

    void tick() {
        // clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update other events like input handling
        glfwPollEvents();
        // put the stuff we’ve been drawing onto the display
        glfwSwapBuffers(window);
    }

    void run() {
        while (!glfwWindowShouldClose(window)) {
            this->tick();
        }
    }

    ~Application() {
        if (window) {
            glfwDestroyWindow(window);
        }

        glfwTerminate();
    }
};


#endif //ZPG_APPLICATION_H
