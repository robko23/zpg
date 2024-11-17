//
// Created by robko on 11/16/24.
//

#pragma once
#include "Camera.h"
#include "Observer.h"
#include "Transformation.h"
#include "drawable/Cube.h"
#include "imgui.h"
#include "shaders/ShaderLightCube.h"
#include "shaders/ShaderLights.h"
#include <glm/glm.hpp>
#include <memory>
#include <stdint.h>

class Light {
  private:
    Cube cube;
    std::shared_ptr<ShaderLightCube> shaderLightCube;
    TransformationBuilder transformations;
    std::shared_ptr<ShaderLights> shaderLightning;
    size_t lightIndex;

    std::optional<std::string> menuTitle = {};
    bool configurable = true;
    bool renderCube = true;

    void renderLightSettings() {
        if (!menuTitle.has_value()) {
            std::stringstream stream;
            stream << "Light settings (" << getId() << ")";
            menuTitle = stream.str();
        }

        const char *title = menuTitle.value().c_str();
        ImGui::Begin(title);
        if (ImGui::ColorPicker3("Light color", &lightColor.x)) {
            update();
        }
        if (ImGui::SliderFloat("Attenuation X", &attenuationX, 0, 1)) {
            update();
        }
        if (ImGui::SliderFloat("Attenuation Y", &attenuationY, 0, 0.5)) {
            update();
        }
        if (ImGui::SliderFloat("Attenuation Z", &attenuationZ, 0, 0.1)) {
            update();
        }
        renderMoreSettings();
        ImGui::End();
    }

  protected:
    glm::vec3 position = glm::vec3(0, 10, 0);
    float attenuationX = 0;
    float attenuationY = 0.1;
    float attenuationZ = 0.02;
    glm::vec3 lightColor = glm::vec3(1);
    glm::vec3 direction = glm::vec3(1);
    float cutoff = 0.8;

    virtual void renderMoreSettings() {}

    LightGLSL &getLight() const {
        return shaderLightning->getLight(lightIndex);
    }

    void update() {
        LightGLSL &light = shaderLightning->getLight(lightIndex);
        light.setPosition(position);
        light.setAttenuation(
            glm::vec3(attenuationX, attenuationY, attenuationZ));
        light.setColor(glm::vec4(lightColor, 1));
        light.setDirection(direction);
		light.setCutoff(cutoff);
        shaderLightCube->setLightColor(light.getColor());
        shaderLightning->updateLight(lightIndex);
        TransformationTranslate *translate =
            dynamic_cast<TransformationTranslate *>(transformations.at(0));
        DEBUG_ASSERT_NOT_NULL(translate);
        translate->setPosition(position);

        shaderLightCube->bind();
        shaderLightCube->modelMatrix(transformations.build());
        shaderLightCube->unbind();
    }

  public:
    explicit Light(const ShaderLoaderV2 &loader, Camera &camera,
                   const std::shared_ptr<ShaderLights> &shaderLights)
        : shaderLightning(shaderLights) {
        shaderLightCube = ShaderLightCube::load(loader).value();
        camera.attach(shaderLightCube);
        camera.projection()->attach(shaderLightCube);

        lightIndex = shaderLightning->addLight(
            LightGLSL(position, glm::vec3(0), glm::vec3(0), glm::vec4(1)));

        transformations =
            TransformationBuilder().translate(position).scale(0.2);
        update();
    }

    void setPosition(glm::vec3 target) {
        position = target;
        update();
    }

    void setColor(glm::vec3 target) {
        lightColor = target;
        update();
    }

    void setConfigurable(bool val) { configurable = val; }

    void setRenderCube(bool val) { renderCube = val; }

    void setDirection(glm::vec3 val) {
        direction = val;
        update();
    }

    void setCutoff(float val) {
        cutoff = val;
        update();
    }

    [[nodiscard]] glm::vec3 getColor() const { return lightColor; }
    [[nodiscard]] glm::vec3 getPosition() const { return position; }

    void render() {
        if (configurable) {
            renderLightSettings();
        }

        if (renderCube) {
            shaderLightCube->bind();
            cube.draw();
            shaderLightCube->unbind();
        }
    }

    [[nodiscard]] virtual const char *getId() const = 0;
};

class PointLight : public Light {
    using Light::Light;
    const char *getId() const override { return "point-light"; }
};

class Flashlight : public Light, public Observer<CameraProperties> {
  private:
    explicit Flashlight(const ShaderLoaderV2 &loader, Camera &camera,
                        const std::shared_ptr<ShaderLights> &shaderLights)
        : Light(loader, camera, shaderLights) {
        getLight().setType(LightType::Reflector);
		attenuationX = 0.1;
		attenuationY = 0.01;
		attenuationZ = 0.005;
		cutoff = 0.9;
    }

  public:
    static std::shared_ptr<Flashlight>
    construct(const ShaderLoaderV2 &loader, Camera &camera,
              const std::shared_ptr<ShaderLights> &shaderLights) {
        auto self = std::shared_ptr<Flashlight>(
            new Flashlight(loader, camera, shaderLights));
        camera.attach(self);
        return self;
    }

    void update(const CameraProperties &action) override {
        setPosition(action.cameraPosition);
        setDirection(action.target);
    }

    void renderMoreSettings() override {
        if (ImGui::SliderFloat("Cutoff", &cutoff, 0.7, 1.05)) {
			Light::update();
        }
    }

    const char *getId() const override { return "flashlight"; }
};
