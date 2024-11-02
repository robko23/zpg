// This includes must be in this exact order, so put them on top to be sure
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include "GLWindow.h"
#include "GLFWContext.h"
#include "gl_info.h"
#include "ShaderLoaderV2.h"

#include "scenes/SceneForest.h"
#include "scenes/SceneFpsDisplay.h"
#include "scenes/SceneSwitcher.h"
#include "scenes/SceneSuzi.h"
#include "scenes/SceneLightningBalls.h"
#include "scenes/SceneTriangle.h"

int main() {
    GLFWcontext::inContext([]() {

        auto window = GLWindow::create("ZPG").value();

        window->inContext([&window]() -> void {
            auto shaderLoader = ShaderLoaderV2("./assets/shaders");
            print_gl_info();
            auto forest = std::make_shared<SceneForest>(window, shaderLoader);
            auto forest2 = std::make_shared<SceneForest>(window, shaderLoader);
            auto fps = std::make_shared<SceneFpsDisplay>(std::move(forest));
            auto suziScene = std::make_shared<SceneSuzi>(window, shaderLoader);
            auto ballsScene = std::make_shared<SceneLightningBalls>(window, shaderLoader);

            auto mainScene = SceneSwitcher();
            mainScene.addScene(forest2);
            mainScene.addScene(fps);
            mainScene.addScene(suziScene);
            mainScene.addScene(ballsScene);
            mainScene.addScene(std::make_shared<SceneTriangle>(window, shaderLoader));

            while (!window->shouldClose()) {
                if (mainScene.shouldExit()) {
                    window->close();
                }
                window->startFrame();
                mainScene.render();
                window->endFrame();
            }
        });

    });

    return EXIT_SUCCESS;
}
