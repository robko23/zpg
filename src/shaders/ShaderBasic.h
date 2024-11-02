#pragma once

#include "../Camera.h"
#include "Shader.h"
#include "../ShaderLoaderV2.h"
#include <optional>
#include <memory>
#include "glm/mat4x4.hpp"
#include "../Projection.h"
#include <glm/gtx/string_cast.hpp>
#include "../Observer.h"

class ShaderBasic
        : public Observer<ViewMatrix>, public Observer<ProjectionMatrix>, public Shader {
private:
    ShaderProgram program;
    ViewMatrix viewMatrix;
    ProjectionMatrix projectionMatrix;

    explicit ShaderBasic(ShaderProgram program) : program(std::move(program)),
                                                  viewMatrix(glm::mat4(1)),
                                                  projectionMatrix(glm::mat4(1)) {
    }

public:
    ShaderBasic(const ShaderBasic &other) = delete;

    ShaderBasic(ShaderBasic &&other) noexcept: program(std::move(other.program)),
                                               viewMatrix(other.viewMatrix),
                                               projectionMatrix(other.projectionMatrix) {}

    static std::optional<std::shared_ptr<ShaderBasic>> load(const ShaderLoaderV2 &loader) {
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

    void update(const ViewMatrix &action) override {
        viewMatrix = action;
    }

    void update(const ProjectionMatrix &action) override {
        projectionMatrix = action;
    }

    void bind() override {
        program.bind();
        program.bindParam("viewMatrix", viewMatrix.viewMatrix);
        program.bindParam("projectionMatrix", projectionMatrix.projectionMatrix);
    }

    void unbind() override {
        program.unbind();
    }
};