#pragma once

#include "../Camera.h"
#include "Shader.h"
#include "../ShaderLoader.h"
#include <optional>
#include <memory>
#include "glm/mat4x4.hpp"
#include "../Projection.h"
#include <glm/gtx/string_cast.hpp>
#include "../Observer.h"

class ShaderLightning
        : public Shader,
          public Observer<ViewMatrix>, public Observer<ProjectionMatrix> {
private:
    ShaderProgram program;
    ViewMatrix viewMatrix;
    ProjectionMatrix projectionMatrix;

    explicit ShaderLightning(ShaderProgram program) : program(std::move(program)),
                                                      viewMatrix(glm::mat4(1)),
                                                      projectionMatrix(glm::mat4(1)) {
    }

public:
    ShaderLightning(const ShaderLightning &other) = delete;

    ShaderLightning(ShaderLightning &&other) noexcept: program(std::move(other.program)),
                                                       viewMatrix(other.viewMatrix),
                                                       projectionMatrix(other.projectionMatrix) {}

    static std::optional<std::shared_ptr<ShaderLightning>> load(const ShaderLoader &loader) {
        auto maybeVertexShader = loader.loadVertex("lightning.glsl");
        if (!maybeVertexShader.has_value()) {
            return {};
        }
        auto maybeFragmentShader = loader.loadFragment("lightning.glsl");
        if (!maybeFragmentShader.has_value()) {
            return {};
        }

        auto maybeShaderProgram = ShaderProgram::link(maybeFragmentShader.value(),
                                                      maybeVertexShader.value());
        if (!maybeShaderProgram.has_value()) {
            return {};
        }

        auto inner = ShaderLightning(maybeShaderProgram.value());
        auto self = std::make_shared<ShaderLightning>(std::move(inner));
        return std::move(self);
    }

    void modelMatrix(glm::mat4 mat) {
        DEBUG_ASSERT(program.isBound());
        program.bindParam("modelMatrix", mat);
    }

    void projection(const PerspectiveProjection &projection) {
        DEBUG_ASSERT(program.isBound());
        program.bindParam("projectionMatrix", projection.getProjectionMatrix());
    }

    void update(const ViewMatrix &action) override {
        viewMatrix = action;
    }

    void update(const ProjectionMatrix &action) override {
        projectionMatrix = action;
    }

    void bind() override {
        program.bind();
        program.bindParam("normalMatrix", glm::mat3(1));
        program.bindParam("viewMatrix", viewMatrix.viewMatrix);
        program.bindParam("projectionMatrix", projectionMatrix.projectionMatrix);
    }

    void unbind() override {
        program.unbind();
    }
};