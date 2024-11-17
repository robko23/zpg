//
// Created by robko on 11/16/24.
//

#pragma once
#include "drawable/Cube.h"
#include <memory>
#include "shaders/ShaderLightCube.h"
#include "shaders/ShaderLights.h"
#include <stdint.h>
#include <glm/glm.hpp>
#include "Transformation.h"

class Light {
private:
    Cube cube;
    std::shared_ptr<ShaderLightCube> shaderLightCube;
    TransformationBuilder transformations;
    std::shared_ptr<ShaderLights> shaderLightning;
    size_t lightIndex;
    glm::vec3 position = glm::vec3(0, 10, 0);
    float attenuationX = 0;
    float attenuationY = 0.1;
    float attenuationZ = 0.02;
    glm::vec3 lightColor = glm::vec3(1);
    glm::vec3 direction = glm::vec3(1);

    std::string menuTitle;
    bool configurable = true;

    void update() {
        LightGLSL &light = shaderLightning->getLight(lightIndex);
        light.setPosition(position);
        light.setAttenuation(glm::vec3(attenuationX, attenuationY, attenuationZ));
        light.setColor(glm::vec4(lightColor, 1));
        light.setDirection(direction);
        shaderLightCube->setLightColor(light.getColor());
        shaderLightning->updateLight(lightIndex);
        TransformationTranslate* translate = dynamic_cast<TransformationTranslate*>(transformations.at(
                0));
        DEBUG_ASSERT_NOT_NULL(translate);
        translate->setPosition(position);

        shaderLightCube->bind();
        shaderLightCube->modelMatrix(transformations.build());
        shaderLightCube->unbind();
    }

    void renderLightSettings() {
        ImGui::Begin(menuTitle.c_str());
        if (ImGui::ColorPicker3("Light color", &lightColor.x)) {
            update();
        }
        if (ImGui::SliderFloat("Attenuation X", &attenuationX, 0, 1)) {
            update();
        }
        if (ImGui::SliderFloat("Attenuation Y", &attenuationY, 0, 1)) {
            update();
        }
        if (ImGui::SliderFloat("Attenuation Z", &attenuationZ, 0, 1)) {
            update();
        }
        ImGui::End();
    }

public:

    explicit Light(
            const ShaderLoaderV2 &loader,
            Camera &camera,
            const std::shared_ptr<ShaderLights> &shaderLights
    )
            : shaderLightning(shaderLights) {
        shaderLightCube = ShaderLightCube::load(loader).value();
        camera.attach(shaderLightCube);
        camera.projection()->attach(shaderLightCube);

        lightIndex = shaderLightning->addLight(
                LightGLSL(position, glm::vec3(0), glm::vec3(0), glm::vec4(1)));

        transformations = TransformationBuilder()
                .translate(position)
                .scale(0.2);
        update();

        std::stringstream stream;
        stream << "Light settings (" << getId() << ")";
        menuTitle = stream.str();
    }

    void setPosition(glm::vec3 target) {
        position = target;
        update();
    }

    [[nodiscard]] glm::vec3 getPosition() const {
        return position;
    }

    void setConfigurable(bool val) {
        configurable = val;
    }

    void render() {
        if (configurable) {
            renderLightSettings();
        }

        shaderLightCube->bind();
        cube.draw();
        shaderLightCube->unbind();
    }

    [[nodiscard]] const char* getId() const {
        return "test";
    }
};
