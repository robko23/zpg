// This includes must be in this exact order, so put them on top to be sure
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL

#include "GLWindow.h"
#include "GLFWContext.h"
#include "gl_info.h"
#include "ShaderLoader.h"

#include "scenes/SceneForest.h"

int main() {
    GLFWcontext::inContext([]() {

        auto window = GLWindow::create("ZPG").value();

        window->inContext([&window]() -> void {
            auto shaderLoader = ShaderLoader("./assets/shaders");
            print_gl_info();

            auto sceneForest = SceneForest(window, shaderLoader);

            while (!window->shouldClose()) {
                if (sceneForest.shouldExit()) {
                    window->close();
                }
                window->startFrame();
                sceneForest.render();
                window->endFrame();
            }
        });

    });

    return EXIT_SUCCESS;
}
