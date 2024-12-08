#pragma once

#include "Camera.h"
#include "Observer.h"
#include "Projection.h"
#include "Transformation.h"
#include <GL/glew.h>

#include "AssetManager.h"
#include "drawable/Cube.h"
#include "shaders/ShaderSkybox.h"
#include <GL/gl.h>
#include <SOIL.h>
#include <memory>

class Skybox : public Observer<CameraProperties> {
  private:
    Cube cube;
    std::shared_ptr<ShaderSkybox> shaderSkybox;
    std::shared_ptr<Cubemap> cubemap;
    TransformationTranslate translate;
    bool follow = true;

    explicit Skybox(Camera &camera, const std::shared_ptr<AssetManager> am)
        : shaderSkybox(ShaderSkybox::load(am).value()),
          cubemap(am->loadCubemap()),
          translate(TransformationTranslate(glm::vec3(0))) {
        camera.attach(shaderSkybox);
        camera.projection()->attach(shaderSkybox);
    }

  public:
    Skybox(Skybox &other) = delete;

    static std::shared_ptr<Skybox>
    construct(Camera &camera, const std::shared_ptr<AssetManager>& am) {
        auto self = std::shared_ptr<Skybox>(new Skybox(camera, am));
        camera.attach(self);
        return self;
    }

    void update(const CameraProperties &props) override {
        if (follow) {
            translate.setPosition(props.cameraPosition);
        }
    }

    void setFollow(bool val) { follow = val; }

    void render() {
        shaderSkybox->bind();
        shaderSkybox->modelMatrix(translate.apply(glm::mat4(1)));
        shaderSkybox->setCubemapId(cubemap->getTextureUnit());
        cube.draw();
        shaderSkybox->unbind();
		glClear(GL_DEPTH_BUFFER_BIT);
    }
};
