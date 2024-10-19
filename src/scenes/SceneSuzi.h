//
// Created by robko on 10/19/24.
//

#pragma once

#include "Scene.h"
#include "../drawable/Suzi.h"
#include <memory>
#include "../Camera.h"
#include "../Projection.h"

class SceneSuzi : public Scene {
    Suzi suzi;
    std::shared_ptr<GLWindow> window;
    std::shared_ptr<ShaderBasic> shaderBasic;
    std::shared_ptr<Projection> projection;
    Camera camera;

    bool running = true;
    bool remount = true;
    double lastMouseX = 0;
    double lastMouseY = 0;

    const float walkingSpeed = 0.07;

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
            window->releaseCursor();
            running = false;
        }
    }

    void handleMouseInput() {
        if (window->mouseX() != lastMouseX || window->mouseY() != lastMouseY) {
            camera.onMouseMove(window->mouseX(), window->mouseY());
            lastMouseX = window->mouseX();
            lastMouseY = window->mouseY();
        }
    }

public:
    explicit SceneSuzi(const std::shared_ptr<GLWindow> &window, const ShaderLoader &loader)
            : suzi(), window(window),
              camera(0.13) {
        auto shader = ShaderBasic::load(loader).value();
        camera.registerCameraObserver(shader);
        shaderBasic = std::move(shader);
        auto perspectiveProjection = std::make_shared<PerspectiveProjection>(window->width(), window->height());
        window->registerResizeCallback(perspectiveProjection);
        projection = perspectiveProjection;
    }

    void render() override {
        if (remount) {
            window->captureCursor();
            remount = false;
        }
        handleSceneKeyInput();
        handleMouseInput();
        shaderBasic->bind();
        shaderBasic->projection(*projection);
        shaderBasic->modelMatrix(glm::mat4(1));
        suzi.draw();
        shaderBasic->unbind();
    }

    bool shouldExit() override {
        bool shouldExit = !running;
        if (shouldExit) {
            running = true;
            remount = true;
        }
        return shouldExit;
    }

    const char* getId() override {
        return "suzi";
    }
};
