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

    bool ambientEnabled = true;
    bool diffuseEnabled = true;
    bool specularEnabled = true;
    bool blinnPhong = true;

    void makeBalls() {
        ballsModel.emplace_back(TransformationBuilder().moveX(3).moveZ(3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(-3).moveZ(3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(3).moveZ(-3).build());
        ballsModel.emplace_back(TransformationBuilder().moveX(-3).moveZ(-3).build());
    }

protected:
    void handleKeyInput() override {
    }

public:
    explicit SceneLightningBalls(const std::shared_ptr<GLWindow> &window,
                                 const ShaderLoaderV2 &loader)
            : BasicScene(window), sphere(), ballsModel() {
        auto shader = ShaderLights::load(loader).value();
        camera.attach(shader);
        camera.projection()->attach(shader);
        shaderLightning = std::move(shader);
        makeBalls();
        resetCamera();

        shaderLightning->setAmbientEnabled(ambientEnabled);
        shaderLightning->setDiffuseEnabled(diffuseEnabled);
        shaderLightning->setSpecularEnabled(specularEnabled);
        shaderLightning->setHalfwayEnabled(blinnPhong);
        auto light = Light(glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec4(1));
        shaderLightning->addLight(light);
        auto material = Material(glm::vec4(0.1), glm::vec4(0.5), glm::vec4(0.7), 32);
        shaderLightning->setMaterial(material);
    }

    void renderScene() override {
        shaderLightning->bind();

        for (const auto &item: ballsModel) {
            shaderLightning->modelMatrix(item);
            sphere.draw();
        }

        shaderLightning->unbind();
    }

    void resetCamera() {
        camera.setPosition(glm::vec3(0, 10, 0));
        camera.setYaw(0);
        camera.setPitch(-90);
        camera.projection()->setFov(60);
    }

    void renderMenu() override {
        ImGui::Begin("Lights settings");
        if (ImGui::Checkbox("Ambient", &ambientEnabled)) {
            shaderLightning->setAmbientEnabled(ambientEnabled);
        }

        if (ImGui::Checkbox("Diffuse", &diffuseEnabled)) {
            shaderLightning->setDiffuseEnabled(diffuseEnabled);
        }

        if (ImGui::Checkbox("Specular", &specularEnabled)) {
            shaderLightning->setSpecularEnabled(specularEnabled);
        }

        if (ImGui::Checkbox("use halfway (blinn phong)", &blinnPhong)) {
            shaderLightning->setHalfwayEnabled(blinnPhong);
        }

//        if (ImGui::ColorEdit3("Ambient color", ambientColor)) {
//            shaderLightning->setAmbientColor(
//                    glm::vec3(ambientColor[0], ambientColor[1], ambientColor[2]));
//        }

        if (ImGui::Button("Reset camera")) {
            resetCamera();
        }
        ImGui::End();
    }

    const char* getId() override {
        return "lightning-balls";
    }
};
