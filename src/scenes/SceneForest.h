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
#include "../shaders/ShaderLightTexture.h"
#include "../shaders/ShaderLights.h"
#include "BasicScene.h"
#include "glm/trigonometric.hpp"
#include "imgui.h"
#include <cmath>
#include <memory>
#include <random>

class ForestFloor {
  private:
    std::shared_ptr<Texture> textureGrass;
    std::shared_ptr<ShaderLightTexture> shaderTexture;
    Cube cube;
    glm::mat4 modelMatrix;

  public:
    explicit ForestFloor(const std::shared_ptr<AssetManager> &am,
                         Camera &camera,
                         const std::shared_ptr<LightsCollection> &lights)
        : textureGrass(am->loadTexture("grass.png")),
          shaderTexture(ShaderLightTexture::load(am).value()) {
        shaderTexture->setLightCollection(lights);

        auto material =
            Material(glm::vec4(0.1), glm::vec4(0.419, 0.678, 0.274, 1),
                     glm::vec4(0.047, 1, 0, 1), 64);
        shaderTexture->setMaterial(material);

        camera.attach(shaderTexture);
        camera.projection()->attach(shaderTexture);
        modelMatrix = TransformationBuilder()
                          .translate(glm::vec3(0))
                          .scale(100, 0.1, 100)
                          .build();

        shaderTexture->bind();
        shaderTexture->setTextureUnitId(textureGrass->getTextureUnit());
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
    std::shared_ptr<LightsCollection> lights;

    std::shared_ptr<ShaderLights> shaderLights;
    std::shared_ptr<ShaderLightCube> shaderLightCube;

    PointLight sun;
    std::shared_ptr<Flashlight> flashlight;
    std::vector<Firefly> fireflies;

    const size_t NUM_FIREFLIES = 2;
    float maxScatterRadius = 50;

    int numberOfTrees = 80;
    std::vector<glm::mat4> treeTrans;

    int numberOfBushes = 50;
    std::vector<glm::mat4> bushesTrans;

    std::shared_ptr<Skybox> skybox;
    bool followSkybox = true;

    ForestFloor floor;

    std::shared_ptr<DynamicModel> houseModel;
    std::shared_ptr<DynamicModel> loginModel;
    glm::mat4 loginModelMatrix;
    std::shared_ptr<Texture> houseTexture;
    std::shared_ptr<ShaderLightTexture> shaderLightsTexture;
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
        : BasicScene(window), lights(std::make_shared<LightsCollection>()),
          shaderLights(ShaderLights::load(loader).value()),
          shaderLightCube(ShaderLightCube::load(loader).value()),
          sun(camera, lights, shaderLightCube),
          flashlight(Flashlight::construct(camera, lights, shaderLightCube)),
          skybox(Skybox::construct(camera, loader, "skybox-night", "png")),
          floor(loader, camera, lights),
          houseModel(loader->loadModel("house.obj")),
          loginModel(loader->loadModel("login.obj")),
          houseTexture(loader->loadTexture("house.png")),
          shaderLightsTexture(ShaderLightTexture::load(loader).value()) {
        shaderLights->setLightCollection(lights);
        shaderLightsTexture->setLightCollection(lights);
        treeTrans = scatterObjects(numberOfTrees);
        bushesTrans = scatterObjects(numberOfBushes);
        camera.attach(shaderLights);
        camera.projection()->attach(shaderLights);

        camera.attach(shaderLightsTexture);
        camera.projection()->attach(shaderLightsTexture);

        shaderLights->applyBlinnPhong();

        sun.setPosition(glm::vec3(0, 10, 0));
        sun.setConfigurable(true);
        sun.setColor(glm::vec3(1));

        flashlight->setRenderCube(false);

        fireflies.reserve(NUM_FIREFLIES);
        for (int i = 0; i < NUM_FIREFLIES; i++) {
            Firefly firefly(camera, lights, window, shaderLightCube);
            firefly.setPosition(glm::vec3(i, 5, i));
            fireflies.emplace_back(std::move(firefly));
        }

        houseModelMatrix = TransformationBuilder()
                               .moveX(70)
                               .rotateY(glm::radians(90.))
                               .scale(3)
                               .build();

        loginModelMatrix = TransformationBuilder().scale(3).moveY(3).build();
    }

    void renderScene() override {
        skybox->render();
        floor.render();

        shaderLightsTexture->bind();
        shaderLightsTexture->setMaterial(houseModel->getMaterial());
        shaderLightsTexture->setTextureUnitId(houseTexture->getTextureUnit());
        shaderLightsTexture->modelMatrix(houseModelMatrix);
        houseModel->draw();
        shaderLightsTexture->unbind();

        shaderLightCube->bind();
        sun.render();
        flashlight->render();
        for (Firefly &firefly : fireflies) {
            firefly.render();
        }
        shaderLightCube->unbind();
        shaderLights->bind();

        shaderLights->setMaterial(
            Material(glm::vec4(0.1), glm::vec4(0.6), glm::vec4(0.6), 64));
        shaderLights->modelMatrix(loginModelMatrix);
        loginModel->draw();

        shaderLights->setMaterial(Material(glm::vec4(0.1),
                                           glm::vec4(0.419, 0.678, 0.274, 1),
                                           glm::vec4(0.047, 1, 0, 1), 64));

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
