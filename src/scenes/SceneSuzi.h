//
// Created by robko on 10/19/24.
//

#pragma once

#include "../Camera.h"
#include "../Projection.h"
#include "../drawable/Suzi.h"
#include "../shaders/ShaderLights.h"
#include "BasicScene.h"
#include <memory>

class SceneSuzi : public BasicScene {
    Suzi suzi;
    //    std::shared_ptr<ShaderBasic> shaderBasic;
    std::shared_ptr<LightsCollection> lights;
    std::shared_ptr<ShaderLights> shader;

  public:
    explicit SceneSuzi(const std::shared_ptr<GLWindow> &window,
                       const std::shared_ptr<AssetManager> &loader)
        : BasicScene(window), suzi(),
          lights(std::make_shared<LightsCollection>()),
          shader(std::move(ShaderLights::load(loader).value())) {
        shader->setLightCollection(lights);
        camera.attach(shader);
        camera.projection()->attach(shader);

        shader->applyBlinnPhong();

        auto light =
            LightGLSL(glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec4(1));
        lights->addLight(light);
        auto material =
            Material(glm::vec4(0.1), glm::vec4(0.1), glm::vec4(0.1), 32);
        shader->setMaterial(material);
    }

    void renderScene() override {
        shader->bind();
        shader->modelMatrix(glm::mat4(1));
        suzi.draw();
        shader->unbind();
    }

    const char *getId() override { return "suzi"; }
};
