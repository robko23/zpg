//
// Created by robko on 10/15/24.
//

#pragma once

#include "../Camera.h"
#include "../GLWindow.h"
#include "../Light.h"
#include "../Skybox.h"
#include "../Transformation.h"
#include "../drawable/Bush.h"
#include "../drawable/Tree.h"
#include "../shaders/ShaderBasicTexture.h"
#include "../shaders/ShaderLights.h"
#include "BasicScene.h"
#include "imgui.h"
#include <cmath>
#include <memory>
#include <random>

class ForestFloor {
  private:
    std::shared_ptr<Texture> textureGrass;
    std::shared_ptr<ShaderBasicTexture> shaderTexture;
    Cube cube;
    glm::mat4 modelMatrix;

  public:
    explicit ForestFloor(const std::shared_ptr<AssetManager> &am,
                         Camera &camera)
        : textureGrass(am->loadTexture("grass.png")),
          shaderTexture(ShaderBasicTexture::load(am).value()) {
        camera.attach(shaderTexture);
        camera.projection()->attach(shaderTexture);
        modelMatrix = TransformationBuilder()
                          .translate(glm::vec3(0))
                          .scale(70, 0.1, 70)
                          .build();

        shaderTexture->bind();
        shaderTexture->setTextureId(textureGrass->getTextureUnit());
        shaderTexture->modelMatrix(modelMatrix);
        shaderTexture->unbind();
    }

    void render() {
        shaderTexture->bind();

        cube.draw();

        shaderTexture->unbind();
    }
};

class SceneForest : public BasicScene {
  private:
    Tree tree;
    Bush bush;
    std::shared_ptr<ShaderLights> shaderLights;
    std::shared_ptr<ShaderLightCube> shaderLightCube;

    PointLight sun;
    std::shared_ptr<Flashlight> flashlight;
    std::vector<Firefly> fireflies;

    const size_t NUM_FIREFLIES = 10;
    float maxScatterRadius = 50;

    int numberOfTrees = 50;
    std::vector<glm::mat4> treeTrans;

    int numberOfBushes = 300;
    std::vector<glm::mat4> bushesTrans;

    std::shared_ptr<Skybox> skybox;
    bool followSkybox = true;

    ForestFloor floor;

    std::shared_ptr<DynamicModel> houseModel;
    std::shared_ptr<Texture> houseTexture;
    std::shared_ptr<ShaderBasicTexture> textureShader;
    glm::mat4 houseModelMatrix;

    void renderMenu() override {
        ImGui::Begin("SceneForest controls");

        int prevNot = numberOfTrees;
        ImGui::SliderInt("Number of trees", &numberOfTrees, 10, 200);
        if (prevNot != numberOfTrees) {
            treeTrans = scatterObjects(numberOfTrees);
        }

        int prevNob = numberOfBushes;
        ImGui::SliderInt("Number of bushes", &numberOfBushes, 50, 2000);
        if (prevNob != numberOfBushes) {
            bushesTrans = scatterObjects(numberOfBushes);
        }

        float prevScatterRadius = maxScatterRadius;
        ImGui::SliderFloat("Scatter radius", &maxScatterRadius, 3, 50);
        if (prevScatterRadius != maxScatterRadius) {
            treeTrans = scatterObjects(numberOfTrees);
            bushesTrans = scatterObjects(numberOfBushes);
        }

        if (ImGui::Checkbox("Follow skybox", &followSkybox)) {
            skybox->setFollow(followSkybox);
        }

        ImGui::End();
    }

    [[nodiscard]] std::vector<glm::mat4> scatterObjects(int num) const {
        auto trans = std::vector<glm::mat4>(num);
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_real_distribution<> angle_distribution(0, 2 * M_PI);
        std::uniform_real_distribution<> radius_distribution(0, 1);

        for (int i = 0; i < num; ++i) {
            double theta = angle_distribution(generator);
            double radius =
                maxScatterRadius * sqrt(radius_distribution(generator));
            auto x = radius * std::cos(theta);
            auto z = radius * std::sin(theta);

            auto modelMatrix = TransformationBuilder()
                                   .rotateY(std::cos(theta))
                                   .moveX(x)
                                   .moveZ(z)
                                   .build();
            trans.emplace_back(modelMatrix);
        }
        return trans;
    }

  public:
    explicit SceneForest(const std::shared_ptr<GLWindow> &window,
                         const std::shared_ptr<AssetManager> &loader)
        : BasicScene(window), shaderLights(ShaderLights::load(loader).value()),
          shaderLightCube(ShaderLightCube::load(loader).value()),
          sun(camera, shaderLights, shaderLightCube),
          flashlight(
              Flashlight::construct(camera, shaderLights, shaderLightCube)),
          skybox(Skybox::construct(camera, loader)), floor(loader, camera),
          houseModel(loader->loadModel("house.obj")),
          houseTexture(loader->loadTexture("house.png")),
          textureShader(ShaderBasicTexture::load(loader).value()) {
        treeTrans = scatterObjects(numberOfTrees);
        bushesTrans = scatterObjects(numberOfBushes);
        camera.attach(shaderLights);
        camera.projection()->attach(shaderLights);

        camera.attach(textureShader);
        camera.projection()->attach(textureShader);

        shaderLights->applyBlinnPhong();

        sun.setPosition(glm::vec3(0, 10, 0));
        sun.setConfigurable(true);
        sun.setColor(glm::vec3(1));

        flashlight->setRenderCube(false);

        auto material =
            Material(glm::vec4(0.1), glm::vec4(0.419, 0.678, 0.274, 1),
                     glm::vec4(0.047, 1, 0, 1), 64);
        shaderLights->setMaterial(material);

        fireflies.reserve(NUM_FIREFLIES);
        for (int i = 0; i < NUM_FIREFLIES; i++) {
            Firefly firefly(camera, shaderLights, window, shaderLightCube);
            firefly.setPosition(glm::vec3(i, 5, i));
            fireflies.emplace_back(std::move(firefly));
        }

        houseModelMatrix = TransformationBuilder()
                               .moveX(55)
                               // 90 deg
                               .rotateY(1.570796)
                               .build();
    }

    void renderScene() override {
        skybox->render();
        floor.render();

        textureShader->bind();
        textureShader->setTextureId(houseTexture->getTextureUnit());
        textureShader->modelMatrix(houseModelMatrix);
        houseModel->draw();
        textureShader->unbind();

        shaderLightCube->bind();
        sun.render();
        flashlight->render();
        for (Firefly &firefly : fireflies) {
            firefly.render();
        }
        shaderLightCube->unbind();
        shaderLights->bind();

        for (const auto &item : treeTrans) {
            shaderLights->modelMatrix(item);
            tree.draw();
        }

        for (const auto &item : bushesTrans) {
            shaderLights->modelMatrix(item);
            bush.draw();
        }

        shaderLights->unbind();
    }

    const char *getId() override { return "forest"; }
};
