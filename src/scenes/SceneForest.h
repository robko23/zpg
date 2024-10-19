//
// Created by robko on 10/15/24.
//

#pragma once

#include "../GLWindow.h"
#include "../shaders/Shader.h"
#include "../Camera.h"
#include "../Transformation.h"
#include <memory>
#include "../assertions.h"
#include "../shaders/ShaderBasic.h"
#include "../ShaderLoader.h"
#include <memory>
#include "../drawable/Tree.h"
#include "imgui.h"

class SceneForest {
private:
    std::shared_ptr<GLWindow> window;
    Tree tree;
    std::shared_ptr<ShaderBasic> shaderBasic;

    Camera camera;
    std::shared_ptr<PerspectiveProjection> projection;
    float walkingSpeed = 0.01;
//    int numOfTrees;

public:
    explicit SceneForest(const std::shared_ptr<GLWindow> &window, const ShaderLoader &loader)
            : window(window), camera(),
              projection(
                      std::make_shared<PerspectiveProjection>(window->width(), window->height())) {
        auto shader = ShaderBasic::load(loader).value();
        camera.registerCameraObserver(shader);
        window->registerResizeCallback(projection);

        shaderBasic = std::move(shader);
    }

    void render() {
//        ImGui::Begin("Hello world");
//        ImGui::Text("Bruh");
//        ImGui::SliderInt("Num of trees", &numOfTrees, 5, 10);
//        ImGui::Text("Ay: %d", numOfTrees);
//        ImGui::End();
        handleKeyInput();
        shaderBasic->bind();

        // TODO: observer here too?
        shaderBasic->projection(*projection);

        shaderBasic->modelMatrix(glm::mat4(1));
        tree.draw();
        shaderBasic->unbind();
        DEBUG_ASSERT(window->isActive());
    }

    void handleKeyInput() {
        if (window->isPressed(GLFW_KEY_W)) {
            camera.moveForward(walkingSpeed);
        }
        if (window->isPressed(GLFW_KEY_S)) {
            camera.moveBack(walkingSpeed);
        }
        if (window->isPressed(GLFW_KEY_A)) {
            camera.moveLeft(walkingSpeed);
        }
        if (window->isPressed(GLFW_KEY_D)) {
            camera.moveRight(walkingSpeed);
        }
    }
};
