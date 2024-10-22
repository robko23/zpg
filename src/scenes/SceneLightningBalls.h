//
// Created by robko on 10/19/24.
//

#pragma once

#include "Scene.h"
#include "../drawable/Sphere.h"
#include <memory>
#include "../Camera.h"
#include "../Projection.h"
#include "../GLWindow.h"
#include "../shaders/ShaderLightning.h"

class SceneLightningBalls : public Scene {
    Sphere sphere;
    std::shared_ptr<GLWindow> window;
    std::shared_ptr<ShaderLightning> shaderLightning;
//    std::shared_ptr<ShaderBasic> shaderLightning;
    std::shared_ptr<PerspectiveProjection> projection;
    Camera camera;
    std::vector<glm::mat4> ballsModel;

    bool running = true;
    bool inMenu = true;
    double lastMouseX = 0;
    double lastMouseY = 0;

    const float walkingSpeed = 0.07;

    float fov = 60;

    void handleSceneKeyInput() {
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
        if (window->isPressedAndClear(GLFW_KEY_ESCAPE)) {
            showMenu();
        }
    }

    void handleMenuKeyInput() {
        if (window->isPressedAndClear(GLFW_KEY_ESCAPE)) {
            hideMenu();
        }
    }

    void handleMouseInput() {
        if (window->mouseX() != lastMouseX || window->mouseY() != lastMouseY) {
            camera.onMouseMove(window->mouseX(), window->mouseY());
            lastMouseX = window->mouseX();
            lastMouseY = window->mouseY();
        }
    }

    void showMenu() {
        window->releaseCursor();
        inMenu = true;
    }

    void hideMenu() {
        window->captureCursor();
        inMenu = false;
    }

    void renderMenu() {
        ImGui::Begin("SceneLightningBalls settings");

        if (ImGui::Button("Resume")) {
            hideMenu();
        }

        if (ImGui::Button("Exit")) {
            running = false;
        }

        ImGui::End();
    }

    void makeBalls() {
        ballsModel.emplace_back(TransformationBuilder().moveX(3).moveZ(3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(-3).moveZ(3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(3).moveZ(-3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(-3).moveZ(-3).build());
    }

public:
    explicit SceneLightningBalls(const std::shared_ptr<GLWindow> &window,
                                 const ShaderLoader &loader)
            : sphere(), window(window),
              camera(0.13), ballsModel() {
        auto shader = ShaderLightning::load(loader).value();
        camera.registerCameraObserver(shader);
        shaderLightning = std::move(shader);
        auto perspectiveProjection = std::make_shared<PerspectiveProjection>(window->width(),
                                                                             window->height());
        window->registerResizeCallback(perspectiveProjection);
        projection = perspectiveProjection;
        makeBalls();
    }

    void render() override {
        if (inMenu) {
            handleMenuKeyInput();
            renderMenu();
        } else {
            handleSceneKeyInput();
            handleMouseInput();
        }
        shaderLightning->bind();
        shaderLightning->projection(*projection);

        for (const auto &item: ballsModel) {
            shaderLightning->modelMatrix(item);
            sphere.draw();
        }


        shaderLightning->unbind();
    }

    bool shouldExit() override {
        bool shouldExit = !running;
        if (shouldExit) {
            running = true;
        }
        return shouldExit;
    }

    const char* getId() override {
        return "lightning-balls";
    }
};
