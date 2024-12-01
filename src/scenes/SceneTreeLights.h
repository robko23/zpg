//
// Created by robko on 11/16/24.
//
#include "../GLWindow.h"
#include "../Light.h"
#include "../drawable/Tree.h"
#include "../shaders/ShaderLights.h"
#include "BasicScene.h"
#include <memory>

// attenuation: 0, 0.008, 0.008
// mat: vec3(13), vec3(107, 173, 70), vec3(12, 255, 0), 64

class SceneTreeLights : public BasicScene {
  private:
    Tree tree;
    std::shared_ptr<ShaderLights> shaderLightning;
    std::shared_ptr<ShaderLightCube> shaderLightCube;
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
        TransformationRotate *tree1ZRotation =
            dynamic_cast<TransformationRotate *>(treeTransformations.at(0));
        DEBUG_ASSERT_NOT_NULL(tree1ZRotation);
        tree1ZRotation->setAngleRadians(tree1ZRotation->getAngleRadians() +
                                        (rotation_speed * window->getDelta()));
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
        if (ImGui::Checkbox("Automatic light movement in X",
                            &autoMoveXEnabled)) {
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
                             const std::shared_ptr<AssetManager> &loader)
        : BasicScene(window),
          shaderLightning(ShaderLights::load(loader).value()),
          shaderLightCube(ShaderLightCube::load(loader).value()),
          pointLight(PointLight(camera, shaderLightning, shaderLightCube)) {
        camera.attach(shaderLightning);
        camera.projection()->attach(shaderLightning);

        updateMaterial();
        shaderLightning->applyBlinnPhong();

        treeTransformations = TransformationBuilder().rotateY(0);
        //        transformationBuilder1 =
        //        std::move(transformationBuilder1.rotateZ(0).moveX(10).moveY(10));
    }

    void renderScene() override {
        renderMaterialSettings();
        renderLightPosSettings();
        if (autoMoveXEnabled) {
            applyLightMovement();
        }
        shaderLightCube->bind();
        pointLight.render();
        shaderLightCube->unbind();

        shaderLightning->bind();
        applyRotation();
        shaderLightning->modelMatrix(treeTransformations.build());
        tree.draw();

        //        shader->modelMatrix(transformationBuilder1.build());
        //        tree.draw();

        shaderLightning->unbind();
    }

    const char *getId() override { return "forest-lights"; }
};
