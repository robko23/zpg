// This includes must be in this exact order, so put them on top to be sure
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL

#include "AssetManager.h"
#include "GLFWContext.h"
#include "GLWindow.h"
#include "gl_info.h"

#include "scenes/SceneForest.h"
#include "scenes/SceneFpsDisplay.h"
#include "scenes/SceneHelloTexture.h"
#include "scenes/SceneLightningBalls.h"
#include "scenes/SceneSuzi.h"
#include "scenes/SceneSwitcher.h"
#include "scenes/SceneTreeLights.h"
#include "scenes/SceneTriangle.h"

int main() {
    GLFWcontext::inContext([]() {
        auto window = GLWindow::create("ZPG").value();

        window->inContext([&window]() -> void {
            auto assetManager = std::make_shared<AssetManager>("./assets");
            print_gl_info();
            auto forest = std::make_shared<SceneForest>(window, assetManager);
            auto forest2 = std::make_shared<SceneForest>(window, assetManager);
            auto fps = std::make_shared<SceneFpsDisplay>(std::move(forest));
            auto suziScene = std::make_shared<SceneSuzi>(window, assetManager);
            auto ballsScene =
                std::make_shared<SceneLightningBalls>(window, assetManager);
            auto basicTextureScene =
                std::make_shared<SceneHeloTexture>(window, assetManager);

            auto mainScene = SceneSwitcher();
            mainScene.addScene(forest2);
            mainScene.addScene(basicTextureScene);
            mainScene.addScene(fps);
            mainScene.addScene(suziScene);
            mainScene.addScene(ballsScene);
            mainScene.addScene(
                std::make_shared<SceneTriangle>(window, assetManager));
            mainScene.addScene(
                std::make_shared<SceneTreeLights>(window, assetManager));

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
