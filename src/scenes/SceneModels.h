//
// Created by robko on 10/19/24.
//

#pragma once

#include "../Camera.h"
#include "../Skybox.h"
#include "../drawable/DynamicModel.h"
#include "../shaders/ShaderBasicTexture.h"
#include "BasicScene.h"
#include <memory>

class SceneModels : public BasicScene {
    std::shared_ptr<DynamicModel> model;
    std::shared_ptr<ShaderBasicTexture> shader;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Skybox> skybox;

  public:
    explicit SceneModels(const std::shared_ptr<GLWindow> &window,
                         const std::shared_ptr<AssetManager> &loader)
        : BasicScene(window), model(loader->loadModel("house.obj")),
          shader(ShaderBasicTexture::load(loader).value()),
          texture(loader->loadTexture("house.png")),
          skybox(Skybox::construct(camera, loader)) {
        camera.attach(shader);
        camera.projection()->attach(shader);
        shader->setTextureId(texture->getTextureUnit());

        shader->bind();
        shader->modelMatrix(glm::mat4(1));
        shader->unbind();
    }

    void renderScene() override {
        skybox->render();

        shader->bind();
        model->draw();
        shader->unbind();
    }

    const char *getId() override { return "models"; }
};
