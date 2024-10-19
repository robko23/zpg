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
#include "Scene.h"
#include "../MathHelpers.h"
#include <random>
#include <cmath>
#include "../drawable/Bush.h"

class SceneForest : public Scene {
private:
    const float MIN_SENSITIVITY = 0.02;
    const float MAX_SENSITIVITY = 0.25;
    const int SENSITIVITY_STEPS = 20;

    const float MIN_WALKING_SPEED = 0.02;
    const float MAX_WALKING_SPEED = 0.25;
    const int WALKING_SPEED_STEPS = 20;

    std::shared_ptr<GLWindow> window;
    Tree tree;
    Bush bush;
    std::shared_ptr<ShaderBasic> shaderBasic;
    int currentSensitivity;
    int currentWalkingSpeed;
    double walkingSpeedCalc;

    Camera camera;
    std::shared_ptr<PerspectiveProjection> projection;

    float maxScatterRadius = 10;

    int numberOfTrees = 50;
    std::vector<glm::mat4> treeTrans;

    int numberOfBushes = 300;
    std::vector<glm::mat4> bushesTrans;

    double lastMouseX = 0;
    double lastMouseY = 0;

    bool inMenu = true;
    bool running = true;

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

    void renderMenu() {
        ImGui::Begin("SceneForest controls");

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

        int prevNot = numberOfTrees;
        ImGui::SliderInt("Number of trees", &numberOfTrees, 10, 200);
        if (prevNot != numberOfTrees) {
            treeTrans = scatterObjects(numberOfTrees);
        }

        int prevNob = numberOfBushes;
        ImGui::SliderInt("Number of bushes", &numberOfBushes, 50, 2000);
        if (prevNob != numberOfBushes) {
            bushesTrans = scatterObjects(numberOfBushes);
        }

        float prevScatterRadius = maxScatterRadius;
        ImGui::SliderFloat("Scatter radius", &maxScatterRadius, 3, 50);
        if (prevScatterRadius != maxScatterRadius) {
            treeTrans = scatterObjects(numberOfTrees);
            bushesTrans = scatterObjects(numberOfBushes);
        }

        if (ImGui::Button("Resume")) {
            hideMenu();
        }

        if (ImGui::Button("Exit")) {
            running = false;
        }
        ImGui::End();
    }

    std::vector<glm::mat4> scatterObjects(int num) const {
        auto trans = std::vector<glm::mat4>(num);
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_real_distribution<> angle_distribution(0, 2*M_PI);
        std::uniform_real_distribution<> radius_distribution(0,1);

        for (int i = 0; i < num; ++i) {
            double theta = angle_distribution(generator);
            double radius = maxScatterRadius * sqrt(radius_distribution(generator));
            auto x = radius * std::cos(theta);
            auto z =  radius * std::sin(theta);

            auto modelMatrix = TransformationBuilder()
                    .rotateY(std::cos(theta))
                    .moveX(x)
                    .moveZ(z)
                    .build();
            trans.emplace_back(modelMatrix);
        }
        return trans;
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
    }

    void handleMouseInput() {
        if (window->mouseX() != lastMouseX || window->mouseY() != lastMouseY) {
            camera.onMouseMove(window->mouseX(), window->mouseY());
            lastMouseX = window->mouseX();
            lastMouseY = window->mouseY();
        }
    }

public:
    explicit SceneForest(const std::shared_ptr<GLWindow> &window, const ShaderLoader &loader)
            : window(window),
              currentSensitivity(5),
              currentWalkingSpeed(10),
              walkingSpeedCalc(0),
              camera(getSensitivity()),
              projection(
                      std::make_shared<PerspectiveProjection>(window->width(), window->height())) {
        calculateWalkingSpeed();
        treeTrans = scatterObjects(numberOfTrees);
        bushesTrans = scatterObjects(numberOfBushes);
        auto shader = ShaderBasic::load(loader).value();
        camera.registerCameraObserver(shader);
        window->registerResizeCallback(projection);

        shaderBasic = std::move(shader);
    }

    void render() override {
        if (inMenu) {
            renderMenu();
            handleMenuKeyInput();
        } else {
            handleSceneKeyInput();
            handleMouseInput();
        }
        shaderBasic->bind();

        shaderBasic->projection(*projection);

        for (const auto &item: treeTrans) {
            shaderBasic->modelMatrix(item);
            tree.draw();
        }

        for (const auto &item: bushesTrans) {
            shaderBasic->modelMatrix(item);
            bush.draw();
        }

        shaderBasic->unbind();
    }

    [[nodiscard]] bool shouldExit() override {
        bool shouldExit = !running;
        if (shouldExit) {
            running = true;
        }
        return shouldExit;
    }

    const char* getId() override {
        return "forest";
    }
};
