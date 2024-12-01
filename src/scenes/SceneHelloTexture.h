//
// Created by robko on 11/2/24.
//

#include "../GLWindow.h"
#include "../Skybox.h"
#include "../Transformation.h"
#include "../drawable/PlaneWithTexture.h"
#include "../shaders/ShaderBasicTexture.h"
#include "Scene.h"
#include <memory.h>

class SceneHeloTexture : public Scene {
    TestModel plane;
    std::shared_ptr<Texture> woodenFence;
    std::shared_ptr<Texture> grass;
    std::shared_ptr<GLWindow> window;
    std::shared_ptr<ShaderBasicTexture> shader;
    Skybox skybox;
    bool running = true;
    TransformationBuilder trans;

    void applyRot() {
        TransformationRotate *rot =
            dynamic_cast<TransformationRotate *>(trans.at(1));
        DEBUG_ASSERT_NOT_NULL(rot);
        rot->setAngleRadians(rot->getAngleRadians() + (1 * window->getDelta()));
    }

    void moveObj1() {
        TransformationTranslate *translate =
            dynamic_cast<TransformationTranslate *>(trans.at(0));
        DEBUG_ASSERT_NOT_NULL(translate);
        translate->setPosition(glm::vec3(0.5, 0, 0));
    }

    void moveObj2() {
        TransformationTranslate *translate =
            dynamic_cast<TransformationTranslate *>(trans.at(0));
        DEBUG_ASSERT_NOT_NULL(translate);
        translate->setPosition(glm::vec3(-0.5, 0, 0));
    }

  public:
    explicit SceneHeloTexture(const std::shared_ptr<GLWindow> &window,
                              const std::shared_ptr<AssetManager> &loader)
        : woodenFence(loader->loadTexture("wooden_fence.png")),
          grass(loader->loadTexture("grass.png")), window(window),
          shader(ShaderBasicTexture::load(loader).value()), skybox(loader) 
		{
        shader->update(CameraProperties::defaultProps());
        shader->update(ProjectionMatrix::_default());
        trans = TransformationBuilder().moveX(0).rotateY(0);
    }

    void render() override {
        skybox.render();
        applyRot();
        if (window->isPressedAndClear(GLFW_KEY_ESCAPE)) {
            running = false;
        }
        shader->bind();
        moveObj1();
        shader->modelMatrix(trans.build());
        shader->setTextureId(woodenFence->getTextureUnit());
        plane.draw();

        moveObj2();
        shader->modelMatrix(trans.build());
        shader->setTextureId(grass->getTextureUnit());
        plane.draw();

        shader->unbind();
    }

    bool shouldExit() override {
        if (!running) {
            running = true;
            return true;
        }
        return false;
    }

    const char *getId() override { return "basic-texture-plane"; }
};
