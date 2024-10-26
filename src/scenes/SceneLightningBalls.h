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
#include "BasicScene.h"

class SceneLightningBalls : public BasicScene {
    Sphere sphere;
    std::shared_ptr<ShaderLightning> shaderLightning;
    std::vector<glm::mat4> ballsModel;

    void makeBalls() {
        ballsModel.emplace_back(TransformationBuilder().moveX(3).moveZ(3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(-3).moveZ(3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(3).moveZ(-3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(-3).moveZ(-3).build());
    }

public:
    explicit SceneLightningBalls(const std::shared_ptr<GLWindow> &window,
                                 const ShaderLoader &loader)
            : BasicScene(window), sphere(), ballsModel() {
        auto shader = ShaderLightning::load(loader).value();
        camera.attach(shader);
        camera.projection()->attach(shader);
        shaderLightning = std::move(shader);
        makeBalls();
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
