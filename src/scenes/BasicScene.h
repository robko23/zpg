//
// Created by robko on 10/26/24.
//

#pragma once

#include "../MathHelpers.h"

class BasicScene : public Scene {
private:
    const float MIN_SENSITIVITY = 0.02;
    const float MAX_SENSITIVITY = 0.25;
    const int SENSITIVITY_STEPS = 20;

    const float MIN_WALKING_SPEED = 0.02;
    const float MAX_WALKING_SPEED = 0.25;
    const int WALKING_SPEED_STEPS = 20;

    bool running = true;
    bool inMenu = true;
    double lastMouseX = 0;
    double lastMouseY = 0;
    float fov = 60;

    int currentSensitivity;
    int currentWalkingSpeed;
    double walkingSpeedCalc;

    void showMenu() {
        inMenu = true;
        window->releaseCursor();
    }

    void hideMenu() {
        inMenu = false;
        window->captureCursor();
    }

    [[nodiscard]] double getSensitivity() const {
        return math::interpolate(static_cast<float>(currentSensitivity), 0.f,
                                 static_cast<float>(SENSITIVITY_STEPS), MIN_SENSITIVITY,
                                 MAX_SENSITIVITY);
    }

    void calculateWalkingSpeed() {
        walkingSpeedCalc = math::interpolate(static_cast<float>(currentWalkingSpeed), 0.f,
                                             static_cast<float>(WALKING_SPEED_STEPS),
                                             MIN_WALKING_SPEED,
                                             MAX_WALKING_SPEED);
    }

    void renderCameraControls() {
        ImGui::Begin("BasicScene controls");

        int prevSens = currentSensitivity;
        ImGui::SliderInt("Sensitivity", &currentSensitivity, 0, SENSITIVITY_STEPS);
        if (prevSens != currentSensitivity) {
            camera.setSensitivity(getSensitivity());
        }
        ImGui::Text("Real Sens: %f", getSensitivity());

        int prevWS = currentWalkingSpeed;
        ImGui::SliderInt("Walking speed", &currentWalkingSpeed, 0, WALKING_SPEED_STEPS);
        if (prevWS != currentWalkingSpeed) {
            calculateWalkingSpeed();
        }
        ImGui::Text("Real walking speed: %f", walkingSpeedCalc);

        float lastFov = fov;
        ImGui::SliderFloat("FOV", &fov, 30, 130);
        if (lastFov != fov) {
            camera.projection()->setFov(fov);
        }

        if (ImGui::Button("Resume")) {
            hideMenu();
        }

        if (ImGui::Button("Exit")) {
            exit();
        }
        ImGui::End();
    }

    void handleMenuKeyInput() {
        if (window->isPressedAndClear(GLFW_KEY_ESCAPE)) {
            hideMenu();
        }
    }

    void handleSceneKeyInput() {
        if (window->isPressed(GLFW_KEY_W)) {
            camera.moveForward(walkingSpeedCalc);
        }
        if (window->isPressed(GLFW_KEY_S)) {
            camera.moveBack(walkingSpeedCalc);
        }
        if (window->isPressed(GLFW_KEY_A)) {
            camera.moveLeft(walkingSpeedCalc);
        }
        if (window->isPressed(GLFW_KEY_D)) {
            camera.moveRight(walkingSpeedCalc);
        }
        if (window->isPressedAndClear(GLFW_KEY_ESCAPE)) {
            showMenu();
        }
        handleKeyInput();
    }

    void handleMouseInput() {
        if (window->mouseX() != lastMouseX || window->mouseY() != lastMouseY) {
            camera.onMouseMove(window->mouseX(), window->mouseY());
            lastMouseX = window->mouseX();
            lastMouseY = window->mouseY();
        }
    }

protected:
    std::shared_ptr<GLWindow> window;
    Camera camera;

    virtual void handleKeyInput() {}

    void exit() {
        running = false;
    }

public:

    explicit BasicScene(const std::shared_ptr<GLWindow> &window) : currentSensitivity(5),
                                                                   currentWalkingSpeed(10),
                                                                   walkingSpeedCalc(0),
                                                                   window(window),
                                                                   camera(getSensitivity(),
                                                                          window) {
        calculateWalkingSpeed();
    }

    void render() final {
        if (inMenu) {
            handleMenuKeyInput();
            renderCameraControls();
            renderMenu();
        } else {
            handleSceneKeyInput();
            handleMouseInput();
        }
        renderScene();
    }

    virtual void renderScene() = 0;

    virtual void renderMenu() {}

    [[nodiscard]] bool shouldExit() final {
        bool shouldExit = !running;
        if (shouldExit) {
            running = true;
        }
        return shouldExit;
    }
};