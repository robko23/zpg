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

class SceneSuzi : public BasicScene {
    Suzi suzi;
    std::shared_ptr<ShaderBasic> shaderBasic;

public:
    explicit SceneSuzi(
            const std::shared_ptr<GLWindow> &window, const ShaderLoaderV2 &loader)
            : BasicScene(window), suzi() {
        auto shader = ShaderBasic::load(loader).value();
        camera.attach(shader);
        camera.projection()->attach(shader);
        shaderBasic = std::move(shader);
    }

    void renderScene() override {
        shaderBasic->bind();
        shaderBasic->modelMatrix(glm::mat4(1));
        suzi.draw();
        shaderBasic->unbind();
    }

    const char* getId() override {
        return "suzi";
    }
};
