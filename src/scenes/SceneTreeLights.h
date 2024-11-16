//
// Created by robko on 11/16/24.
//
#include "BasicScene.h"
#include "../drawable/Tree.h"
#include "../GLWindow.h"
#include "../ShaderLoaderV2.h"
#include "../shaders/ShaderLights.h"
#include <memory>
#include <random>
#include "../drawable/Cube.h"
#include "../shaders/ShaderLightCube.h"

class PointLight {
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

    void update() {
        Light &light = shaderLightning->getLight(lightIndex);
        light.setPosition(position);
        light.setAttenuation(glm::vec3(attenuationX, attenuationY, attenuationZ));
        light.setColor(glm::vec4(lightColor, 1));
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
        ImGui::Begin("Light settings");
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

    explicit PointLight(
            const ShaderLoaderV2 &loader,
            Camera &camera,
            const std::shared_ptr<ShaderLights> &shaderLights
    )
            : shaderLightning(shaderLights) {
        shaderLightCube = ShaderLightCube::load(loader).value();
        camera.attach(shaderLightCube);
        camera.projection()->attach(shaderLightCube);

        lightIndex = shaderLightning->addLight(
                Light(position, glm::vec3(0), glm::vec3(0), glm::vec4(1)));

        transformations = TransformationBuilder()
                .translate(position)
                .scale(0.2);
        update();
    }

    void setPosition(glm::vec3 target) {
        position = target;
        update();
    }

    glm::vec3 getPosition() const {
        return position;
    }

    void render() {
        renderLightSettings();

        shaderLightCube->bind();
        cube.draw();
        shaderLightCube->unbind();
    }
};
// attenuation: 0, 0.008, 0.008
// mat: vec3(13), vec3(107, 173, 70), vec3(12, 255, 0), 64

class SceneTreeLights : public BasicScene {
private:
    Tree tree;
    std::shared_ptr<ShaderLights> shaderLightning;
    TransformationBuilder treeTransformations;
    PointLight pointLight;
//    TransformationBuilder transformationBuilder1;
    float rotation_speed = 1;
    float lightMovementSpeed = 6;
    uint8_t lightMovementDirection = 0;

    glm::vec3 ambient = glm::vec3(0.1);
    glm::vec3 diffuse = glm::vec3(0.2);
    glm::vec3 specular = glm::vec3(0.3);
    float shininess = 8;

    float lightX = 0;
    float lightY = 10;
    float lightZ = 0;
    bool autoMoveXEnabled = false;

    void applyRotation() {
        TransformationRotate* tree1ZRotation = dynamic_cast<TransformationRotate*>(
                treeTransformations.at(0));
        DEBUG_ASSERT_NOT_NULL(tree1ZRotation);
        tree1ZRotation->setAngleRadians(
                tree1ZRotation->getAngleRadians() +
                (rotation_speed * window->getDelta())
        );
    }

    void applyLightMovement() {
        if (0 == lightMovementDirection) {
            lightX += window->getDelta() * lightMovementSpeed;
            if (lightX > 15) {
                lightMovementDirection = 1;
            }
        } else if (1 == lightMovementDirection) {
            lightX -= window->getDelta() * lightMovementSpeed;
            if (lightX < -15) {
                lightMovementDirection = 0;
            }
        }
        updateLightPos();
    }

    void updateMaterial() {
        auto material = Material(glm::vec4(ambient, 1), glm::vec4(diffuse, 1),
                                 glm::vec4(specular, 1), shininess);
        shaderLightning->setMaterial(material);
    }

    void updateLightPos() {
        pointLight.setPosition(glm::vec3(lightX, lightY, lightZ));
    }

    void renderMaterialSettings() {
        ImGui::Begin("Material settings");
        if (ImGui::ColorPicker3("Ambient", &ambient.x)) {
            updateMaterial();
        }
        if (ImGui::ColorPicker3("Diffuse", &diffuse.x)) {
            updateMaterial();
        }
        if (ImGui::ColorPicker3("Specular", &specular.x)) {
            updateMaterial();
        }
        if (ImGui::SliderFloat("Shininess", &shininess, 1, 64)) {
            updateMaterial();
        }
        ImGui::End();
    }

    void renderLightPosSettings() {
        ImGui::Begin("Light position");
        if (ImGui::Checkbox("Automatic light movement in X", &autoMoveXEnabled)) {
            updateLightPos();
        }
        if (ImGui::SliderFloat("Light X", &lightX, -20, 20)) {
            updateLightPos();
        }
        if (ImGui::SliderFloat("Light Y", &lightY, 0, 10)) {
            updateLightPos();
        }
        if (ImGui::SliderFloat("Light Z", &lightZ, -20, 20)) {
            updateLightPos();
        }
        ImGui::End();
    }

public:
    explicit SceneTreeLights(const std::shared_ptr<GLWindow> &window,
                             const ShaderLoaderV2 &loader)
            : BasicScene(window),
              shaderLightning(ShaderLights::load(loader).value()),
              pointLight(PointLight(loader, camera, shaderLightning)) {
        camera.attach(shaderLightning);
        camera.projection()->attach(shaderLightning);

        updateMaterial();
        shaderLightning->applyBlinnPhong();

        treeTransformations = TransformationBuilder().rotateY(0);
//        transformationBuilder1 = std::move(transformationBuilder1.rotateZ(0).moveX(10).moveY(10));
    }

    void renderScene() override {
        renderMaterialSettings();
        renderLightPosSettings();
        if (autoMoveXEnabled) {
            applyLightMovement();
        }
        pointLight.render();

        shaderLightning->bind();
        applyRotation();
        shaderLightning->modelMatrix(treeTransformations.build());
        tree.draw();

//        shader->modelMatrix(transformationBuilder1.build());
//        tree.draw();

        shaderLightning->unbind();
    }

    const char* getId() override {
        return "forest-lights";
    }
};
