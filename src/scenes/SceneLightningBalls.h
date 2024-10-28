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
#include "../shaders/ShaderLights.h"
#include "BasicScene.h"

class SceneLightningBalls : public BasicScene {
    Sphere sphere;
    std::shared_ptr<ShaderLights> shaderLightning;
    std::vector<glm::mat4> ballsModel;

    void makeBalls() {
        ballsModel.emplace_back(TransformationBuilder().moveX(3).moveZ(3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(-3).moveZ(3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(3).moveZ(-3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(-3).moveZ(-3).build());
    }

protected:
    void handleKeyInput() override {
//        if(window->isPressedAndClear(GLFW_KEY_R)) {
//            shaderLightning->addLight();
//        }
    }

public:
    explicit SceneLightningBalls(const std::shared_ptr<GLWindow> &window,
                                 const ShaderLoader &loader)
            : BasicScene(window), sphere(), ballsModel() {
        auto shader = ShaderLights::load(loader).value();
        camera.attach(shader);
        camera.projection()->attach(shader);
        shaderLightning = std::move(shader);
        makeBalls();
        camera.setPosition(glm::vec3(0, 10, 0));
        camera.setYaw(0);
        camera.setPitch(-90);
    }

    void renderScene() override {
        shaderLightning->bind();

        for (const auto &item: ballsModel) {
            shaderLightning->modelMatrix(item);
            sphere.draw();
        }

        shaderLightning->unbind();
    }

    const char* getId() override {
        return "lightning-balls";
    }
};
