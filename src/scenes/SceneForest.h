//
// Created by robko on 10/15/24.
//

#pragma once

#include "../Camera.h"
#include "../GLWindow.h"
#include "../Light.h"
#include "../ShaderLoaderV2.h"
#include "../Transformation.h"
#include "../drawable/Bush.h"
#include "../drawable/Tree.h"
#include "../shaders/ShaderLights.h"
#include "BasicScene.h"
#include "imgui.h"
#include <cmath>
#include <memory>
#include <random>

class SceneForest : public BasicScene {
  private:
    Tree tree;
    Bush bush;
    std::shared_ptr<ShaderLights> shaderLights;

    PointLight sun;
    std::shared_ptr<Flashlight> flashlight;

    float maxScatterRadius = 10;

    int numberOfTrees = 50;
    std::vector<glm::mat4> treeTrans;

    int numberOfBushes = 300;
    std::vector<glm::mat4> bushesTrans;

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
                         const ShaderLoaderV2 &loader)
        : BasicScene(window), shaderLights(ShaderLights::load(loader).value()),
          sun(loader, camera, shaderLights),
          flashlight(Flashlight::construct(loader, camera, shaderLights)) {
        treeTrans = scatterObjects(numberOfTrees);
        bushesTrans = scatterObjects(numberOfBushes);
        camera.attach(shaderLights);
        camera.projection()->attach(shaderLights);

        shaderLights->applyBlinnPhong();

        sun.setPosition(glm::vec3(0, 10, 0));
        sun.setConfigurable(true);
        sun.setColor(glm::vec3(1));

		flashlight->setRenderCube(false);

        auto material =
            Material(glm::vec4(0.1), glm::vec4(0.419, 0.678, 0.274, 1),
                     glm::vec4(0.047, 1, 0, 1), 64);
        shaderLights->setMaterial(material);
    }

    void renderScene() override {
		sun.render();
		flashlight->render();
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
