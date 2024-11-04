//
// Created by robko on 10/19/24.
//

#pragma once

#include "Scene.h"
#include "../drawable/Suzi.h"
#include <memory>
#include "../Camera.h"
#include "../Projection.h"
#include "BasicScene.h"
#include "../shaders/ShaderLights.h"

class SceneSuzi : public BasicScene {
    Suzi suzi;
//    std::shared_ptr<ShaderBasic> shaderBasic;
    std::shared_ptr<ShaderLights> shader;

public:
    explicit SceneSuzi(
            const std::shared_ptr<GLWindow> &window, const ShaderLoaderV2 &loader)
            : BasicScene(window), suzi() {
        shader = std::move(ShaderLights::load(loader).value());
        camera.attach(shader);
        camera.projection()->attach(shader);

        shader->setAmbientColor(
                glm::vec3(0.1, 0.1, 0.1));
        shader->setAmbientEnabled(true);
        shader->setDiffuseEnabled(true);
        shader->setSpecularEnabled(true);
        shader->setHalfwayEnabled(true);
    }

    void renderScene() override {
        shader->bind();
        shader->modelMatrix(glm::mat4(1));
        shader->cameraPosition(camera.getPosition());
        suzi.draw();
        shader->unbind();
    }

    const char* getId() override {
        return "suzi";
    }
};
