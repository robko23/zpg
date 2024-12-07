//
// Created by robko on 11/16/24.
//

#pragma once

#include "../AssetManager.h"
#include "../Camera.h"
#include "../Projection.h"
#include "Shader.h"

// Helper for string literals
template <std::size_t N> struct StringLiteral {
    consteval StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N];
};

/*
 Basic functionality that almost every shader needs.
 Functions:
 Loading itself from the disk,
 updating view and projection matrix,
 binding and unbinding,
 processing modelMatrix

 You can use it by extending this class like this:
 ```cpp
 class Example:
     public ShaderCommon<Example, "example.glsl", "example.glsl"> {...}
```

 You can optionally change the names of uniform variables using additional
template parameters. Template parameters order and their default values:
 "modelMatrix", "viewMatrix", "projectionMatrix"
 */
template <typename Self, StringLiteral VertexName, StringLiteral FragmentName,
          StringLiteral ModelMatrixUniformName = "modelMatrix",
          StringLiteral ViewMatrixUniformName = "viewMatrix",
          StringLiteral ProjectionMatrixUniformName = "projectionMatrix">
class ShaderCommon : public Observer<CameraProperties>,
                     public Observer<ProjectionMatrix>,
                     public Shader {
  protected:
    ShaderProgram program;

    explicit ShaderCommon(ShaderProgram program)
        : program(std::move(program)) {}

    virtual void onCameraPositionChange(glm::vec3 cameraPosition) {}

  public:
    static std::optional<std::shared_ptr<Self>>
    load(const std::shared_ptr<AssetManager> &loader) {
        auto maybeVertexShader = loader->loadVertex(VertexName.value);
        if (!maybeVertexShader.has_value()) {
            return {};
        }
        auto maybeFragmentShader = loader->loadFragment(FragmentName.value);
        if (!maybeFragmentShader.has_value()) {
            return {};
        }

        auto maybeShaderProgram = ShaderProgram::link(
            maybeFragmentShader.value(), maybeVertexShader.value());
        if (!maybeShaderProgram.has_value()) {
            return {};
        }

        auto inner = Self(std::move(maybeShaderProgram.value()));
        auto self = std::make_shared<Self>(std::move(inner));
        return std::move(self);
    }

    void modelMatrix(glm::mat4 mat) {
        DEBUG_ASSERT(program.isBound());
        program.bindParam(ModelMatrixUniformName.value, mat);
    }

    void update(const CameraProperties &action) override {
        this->bind();
        program.bindParam(ViewMatrixUniformName.value, action.viewMatrix);
        this->onCameraPositionChange(action.cameraPosition);
        this->unbind();
    }

    void update(const ProjectionMatrix &action) override {
        this->bind();
        program.bindParam(ProjectionMatrixUniformName.value,
                          action.projectionMatrix);
        this->unbind();
    }

    void bind() override { program.bind(); }

    void unbind() override { program.unbind(); }

    bool isBound() override { return program.isBound(); }
};
