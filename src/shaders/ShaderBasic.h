#pragma once

#include "../Camera.h"
#include "Shader.h"
#include "../ShaderLoader.h"
#include "../CameraObserver.h"
#include <optional>
#include <memory>
#include "glm/mat4x4.hpp"
#include "../Projection.h"
#include <glm/gtx/string_cast.hpp>

class ShaderBasic : public CameraObserver, public Shader {
private:
    ShaderProgram program;
    glm::mat4 currentCamera;

    explicit ShaderBasic(ShaderProgram program) : program(std::move(program)),
                                                  currentCamera(glm::mat4(1)) {
    }

    inline void updateCamera() {
        DEBUG_ASSERT(program.isBound());
        program.bindParam("cameraMatrix", currentCamera);
    }

public:
    ShaderBasic(const ShaderBasic &other) = delete;

    ShaderBasic(ShaderBasic &&other) noexcept: program(std::move(other.program)),
                                               currentCamera(other.currentCamera)
                                               {}

    static std::optional<std::shared_ptr<ShaderBasic>> load(const ShaderLoader &loader) {
        auto maybeVertexShader = loader.loadVertex("basic.glsl");
        if (!maybeVertexShader.has_value()) {
            return {};
        }
        auto maybeFragmentShader = loader.loadFragment("basic.glsl");
        if (!maybeFragmentShader.has_value()) {
            return {};
        }

        auto maybeShaderProgram = ShaderProgram::link(maybeFragmentShader.value(),
                                                      maybeVertexShader.value());
        if (!maybeShaderProgram.has_value()) {
            return {};
        }

        auto inner = ShaderBasic(maybeShaderProgram.value());
        auto self = std::make_shared<ShaderBasic>(std::move(inner));
        return std::move(self);
    }

    void modelMatrix(glm::mat4 mat) {
        DEBUG_ASSERT(program.isBound());
        program.bindParam("modelMatrix", mat);
    }

    void projection(const Projection& projection) {
        DEBUG_ASSERT(program.isBound());
        program.bindParam("projectionMatrix", projection.getProjectionMatrix());
    }

    void onCameraChange(glm::mat4 mat4) override {
        currentCamera = mat4;
    }

    void bind() override {
        program.bind();
        updateCamera();
    }

    void unbind() override {
        program.unbind();
    }

    bool eq(CameraObserver &other) override {
        auto otherShader = dynamic_cast<ShaderBasic*>(&other);
        if (nullptr == otherShader) {
            return false;
        }
        return otherShader->program == this->program;
    }
};