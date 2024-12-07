//
// Created by robko on 11/16/24.
//

#pragma once

#include "Camera.h"
#include "GLWindow.h"
#include "Observer.h"
#include "Transformation.h"
#include "assertions.h"
#include "drawable/Cube.h"
#include "imgui.h"
#include "shaders/ShaderLightCube.h"
#include "shaders/ShaderLights.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <random>
#include <cstdint>

class Light {
  protected:
    glm::vec3 position = glm::vec3(0, 10, 0);
    TransformationBuilder transformations;
    float attenuationX = 0;
    float attenuationY = 0.1;
    float attenuationZ = 0.02;
    glm::vec3 lightColor = glm::vec3(1);
    glm::vec3 direction = glm::vec3(1);
    float cutoff = 0.8;

  private:
    Cube cube;
    std::shared_ptr<ShaderLightCube> shaderLightCube;
    std::shared_ptr<ShaderLights> shaderLightning;
    size_t lightIndex = SIZE_MAX;

    std::optional<std::string> menuTitle = {};
    bool configurable = true;
    bool renderCube = true;
    bool enabled = true;
    LightType prevType = LightType::None;

    void renderLightSettings() {
        if (!menuTitle.has_value()) {
            std::stringstream stream;
            stream << "Light settings (" << getId() << ")";
            menuTitle = stream.str();
        }

        const char *title = menuTitle.value().c_str();
        ImGui::Begin(title);
        if (ImGui::Checkbox("Enabled", &enabled)) {
            setEnabled(enabled);
        }
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

    LightGLSL &getLight() const {
        return shaderLightning->getLight(lightIndex);
    }

  protected:
    virtual void renderMoreSettings() {}

    void update() {
        DEBUG_ASSERTF(lightIndex != SIZE_MAX,
                      "Use of unitialized or moved light");
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
    }

  public:
    explicit Light(Camera &camera,
                   const std::shared_ptr<ShaderLights> &shaderLights,
                   const std::shared_ptr<ShaderLightCube> &shaderLightCube)
        : transformations(
              TransformationBuilder().translate(position).scale(0.2)),
          shaderLightCube(shaderLightCube), shaderLightning(shaderLights) {
        camera.attach(shaderLightCube);
        camera.projection()->attach(shaderLightCube);

        lightIndex = shaderLights->addLight(
            LightGLSL(position, glm::vec3(0), glm::vec3(0), glm::vec4(1)));
        DEBUG_ASSERT(lightIndex != SIZE_MAX);
        update();
    }

    Light(Light &other) = delete;
    Light(Light &&other) noexcept
        : position(other.position),
          transformations(std::move(other.transformations)),
          attenuationX(other.attenuationX), attenuationY(other.attenuationY),
          attenuationZ(other.attenuationZ), lightColor(other.lightColor),
          direction(other.direction), cutoff(other.cutoff),
          cube(std::move(other.cube)),
          shaderLightCube(std::move(other.shaderLightCube)),
          shaderLightning(std::move(other.shaderLightning)),
          lightIndex(other.lightIndex), menuTitle(std::move(other.menuTitle)),
          configurable(other.configurable), renderCube(other.renderCube),
          enabled(other.enabled), prevType(other.prevType) {
        other.lightIndex = SIZE_MAX;
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

    void setEnabled(bool val) {
        LightGLSL &light = getLight();
        if (val) {
            light.setType(prevType);
            enabled = true;
        } else {
            prevType = light.getType();
            light.setType(LightType::None);
            enabled = false;
        }
        update();
    }

    void setType(LightType type) {
        getLight().setType(type);
        prevType = type;
    }

    [[nodiscard]] glm::vec3 getColor() const { return lightColor; }

    [[nodiscard]] glm::vec3 getPosition() const { return position; }

    [[nodiscard]] bool isEnabled() const { return enabled; }

    virtual void render() {
        DEBUG_ASSERTF(lightIndex != SIZE_MAX,
                      "Use of unitialized or moved light");
        if (configurable) {
            renderLightSettings();
        }

        if (renderCube) {
            DEBUG_ASSERT(shaderLightCube->isBound())
            shaderLightCube->modelMatrix(transformations.build());
            shaderLightCube->setLightColor(getLight().getColor());
            cube.draw();
        }
    }

    [[nodiscard]] virtual const char *getId() const = 0;

    virtual ~Light() {
        if (lightIndex != SIZE_MAX) {
            shaderLightning->removeLight(lightIndex);
        }
    };
};

class PointLight : public Light {
    using Light::Light;

    const char *getId() const override { return "point-light"; }
};

class Flashlight : public Light, public Observer<CameraProperties> {
  private:
    explicit Flashlight(Camera &camera,
                        const std::shared_ptr<ShaderLights> &shaderLights,
                        const std::shared_ptr<ShaderLightCube> &shaderLightCube)
        : Light(camera, shaderLights, shaderLightCube) {
        setType(LightType::Reflector);
        attenuationX = 0.1;
        attenuationY = 0.01;
        attenuationZ = 0.005;
        cutoff = 0.9;
    }

  public:
    static std::shared_ptr<Flashlight>
    construct(Camera &camera, const std::shared_ptr<ShaderLights> &shaderLights,
              const std::shared_ptr<ShaderLightCube> &shaderLightCube) {
        auto self = std::shared_ptr<Flashlight>(
            new Flashlight(camera, shaderLights, shaderLightCube));
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

class Firefly : public Light {
  private:
    std::shared_ptr<GLWindow> window;
    glm::vec3 direction = glm::vec3(1);
    std::mt19937 generator;
    std::uniform_real_distribution<> distribution =
        std::uniform_real_distribution<>(-1, 1);

    glm::vec3 targetDirection = glm::vec3(1.0f);
    float directionChangeInterval = 1.0f; // Seconds between direction changes
    float timeSinceLastChange = 0.0f;
    float maxY = 4;
    float minY = 1;

    void applyWanderingMovement() {
        glm::vec3 position = getPosition();
        // Update direction every directionChangeInterval seconds
        timeSinceLastChange += window->getDelta();
        if (timeSinceLastChange >= directionChangeInterval) {
            // Generate a new random target direction
            targetDirection = glm::normalize(
                glm::vec3(distribution(generator), distribution(generator),
                          distribution(generator)));

            // Check Y boundaries and adjust target direction
            float currentY = position.y;
            if (currentY >= maxY) {
                // Prefer to move down if near the ceiling
                targetDirection.y = glm::min(targetDirection.y, -0.5f);
            } else if (currentY <= minY) {
                // Prefer to move up if near the floor
                targetDirection.y = glm::max(targetDirection.y, 0.5f);
            }

            timeSinceLastChange = 0.0f;
        }

        // Smoothly move towards the target direction
        float interpolationSpeed = 0.05f; // Smaller values are smoother
        direction = glm::normalize(
            glm::mix(direction, targetDirection, interpolationSpeed));

        // Apply movement with current direction
        auto moveBy = static_cast<float>(window->getDelta()) * direction;

        // Check and clamp Y value after movement
        glm::vec3 newPosition = position + moveBy;
        newPosition.y = glm::clamp(newPosition.y, minY, maxY);
        setPosition(newPosition);
    }

  public:
    explicit Firefly(Camera &camera,
                     const std::shared_ptr<ShaderLights> &shaderLights,
                     const std::shared_ptr<GLWindow> &window,
                     const std::shared_ptr<ShaderLightCube> &shaderLightCube)
        : Light(camera, shaderLights, shaderLightCube),
          window(std::move(window)) {
        std::random_device randomDevice;
        std::mt19937 seedGenerator(randomDevice());
        std::uniform_real_distribution<> seed_distribution(0, 9348012495843);
        double seed = seed_distribution(seedGenerator);
        generator.seed(static_cast<float>(seed));

        TransformationScale *scale =
            dynamic_cast<TransformationScale *>(transformations.at(1));
        DEBUG_ASSERT_NOT_NULL(scale);
        scale->setScale(0.05);
        setConfigurable(false);
        setRenderCube(true);
        float randomX = distribution(generator) * 10;
        float randomZ = distribution(generator) * 10;
        setPosition(glm::vec3(randomX, 2, randomZ));
        attenuationZ = 0.2;
        attenuationY = 0.4;
        attenuationX = 0.7;
        setColor(glm::vec3(1, 1, 0.3));
    }

    void render() override {
        applyWanderingMovement();
        Light::render();
    }

    [[nodiscard]] const char *getId() const override { return "firefly"; }
};
