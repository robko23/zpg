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
    Camera camera;

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
        ImGui::Begin("SceneSuzi settings");

        float lastFov = fov;
        ImGui::SliderFloat("FOV", &fov, 30, 130);
        if (lastFov != fov) {
            camera.projection()->setFov(fov);
        }

        if (ImGui::Button("Resume")) {
            hideMenu();
        }

        if (ImGui::Button("Exit")) {
            running = false;
        }

        ImGui::End();
    }

public:
    explicit SceneSuzi(const std::shared_ptr<GLWindow> &window, const ShaderLoader &loader)
            : suzi(), window(window),
              camera(0.13, window) {
        auto shader = ShaderBasic::load(loader).value();
        camera.attach(shader);
        camera.projection()->attach(shader);
        shaderBasic = std::move(shader);
    }

    void render() override {
        if (inMenu) {
            handleMenuKeyInput();
            renderMenu();
        } else {
            handleSceneKeyInput();
            handleMouseInput();
        }
        shaderBasic->bind();
        shaderBasic->modelMatrix(glm::mat4(1));
        suzi.draw();
        shaderBasic->unbind();
    }

    bool shouldExit() override {
        bool shouldExit = !running;
        if (shouldExit) {
            running = true;
        }
        return shouldExit;
    }

    const char* getId() override {
        return "suzi";
    }
};
