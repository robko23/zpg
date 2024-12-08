//
// Created by robko on 11/2/24.
//

#include "../GLWindow.h"
#include "../Skybox.h"
#include "../Transformation.h"
#include "../drawable/PlaneWithTexture.h"
#include "../shaders/ShaderBasicTexture.h"
#include "BasicScene.h"
#include <memory.h>
#include <memory>

class SceneHeloTexture : public BasicScene {
    TestModel plane;
    std::shared_ptr<Texture> woodenFence;
    std::shared_ptr<Texture> grass;
    std::shared_ptr<GLWindow> window;
    std::shared_ptr<ShaderBasicTexture> shader;
    std::shared_ptr<Skybox> skybox;
    TransformationBuilder trans;

    void applyRot() {
        auto *rot = dynamic_cast<TransformationRotate *>(trans.at(1));
        DEBUG_ASSERT_NOT_NULL(rot);
        rot->setAngleRadians(rot->getAngleRadians() + (1 * window->getDelta()));
    }

    void moveObj1() {
        auto *translate = dynamic_cast<TransformationTranslate *>(trans.at(0));
        DEBUG_ASSERT_NOT_NULL(translate);
        translate->setPosition(glm::vec3(0.5, 0, 0));
    }

    void moveObj2() {
        auto *translate = dynamic_cast<TransformationTranslate *>(trans.at(0));
        DEBUG_ASSERT_NOT_NULL(translate);
        translate->setPosition(glm::vec3(-0.5, 0, 0));
    }

  public:
    explicit SceneHeloTexture(const std::shared_ptr<GLWindow> &window,
                              const std::shared_ptr<AssetManager> &loader)
        : BasicScene(window),
          woodenFence(loader->loadTexture("wooden_fence.png")),
          grass(loader->loadTexture("grass.png")), window(window),
          shader(ShaderBasicTexture::load(loader).value()),
          skybox(Skybox::construct(camera, loader, "skybox-bright", "jpg")) {
        shader->update(CameraProperties::defaultProps());
        shader->update(ProjectionMatrix::_default());
        trans = TransformationBuilder().moveX(0).rotateY(0);
    }

    void renderScene() override {
        skybox->render();

        applyRot();

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

    const char *getId() override { return "basic-texture-plane"; }
};
