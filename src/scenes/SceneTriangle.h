//
// Created by robko on 11/2/24.
//

#include "Scene.h"
#include <memory.h>
#include "../GLWindow.h"
#include "../drawable/Triangle.h"
#include "../shaders/ShaderBasic.h"

class SceneTriangle : public Scene {
    Triangle triangle;
    std::shared_ptr<GLWindow> window;
    std::shared_ptr<ShaderBasic> shader;
    bool running = true;
public:

    explicit SceneTriangle(const std::shared_ptr<GLWindow> &window, const ShaderLoaderV2 &loader)
            : window(window) {
        shader = ShaderBasic::load(loader).value();
        shader->update(CameraProperties::defaultProps());
        shader->update(ProjectionMatrix::_default());
    }

    void render() override {
        if (window->isPressedAndClear(GLFW_KEY_ESCAPE)) {
            running = false;
        }
        shader->bind();
        shader->modelMatrix(glm::mat4(1));
        triangle.draw();
        shader->unbind();
    }

    bool shouldExit() override {
        if (!running) {
            running = true;
            return true;
        }
        return false;
    }

    const char* getId() override {
        return "basic-triangle";
    }
};
