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
#include "../gl_utils.h"

template<typename Inner>
class SSBO {
    static_assert(alignof(Inner) == 16);
    static_assert(sizeof(GLuint) == sizeof(unsigned int));
private:
    // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Shader_storage_blocks
    // https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
    GLuint m_ssboId = 0;
    GLuint m_binding;
    std::vector<Inner> m_objects;
public:
    SSBO(const SSBO &other) = delete;

    SSBO(SSBO &&other) noexcept: m_ssboId(other.m_ssboId),
                                 m_binding(other.m_binding),
                                 m_objects(std::move(other.m_objects)) {
        other.m_ssboId = 0;
    }

    explicit SSBO(GLuint binding) : m_binding(binding) {
        glGenBuffers(1, &m_ssboId);
        DEBUG_ASSERT(0 != m_ssboId);
    }

    // Sends array of objects into the ssbo buffer
    void send() {
        DEBUG_ASSERT(0 != m_ssboId);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboId);
        gl::assertNoError();

        size_t allocSize = m_objects.size() * sizeof(Inner);
        glBufferData(GL_SHADER_STORAGE_BUFFER, allocSize, m_objects.data(), GL_DYNAMIC_DRAW);
        gl::assertNoError();

#ifdef DEBUG_ASSERTIONS
        // unbind the buffer
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        gl::assertNoError();
#endif
    }

    // Binds the buffer to the binding used in GLSL shader
    void bind() {
        DEBUG_ASSERT(0 != m_ssboId);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_binding, m_ssboId);
        gl::assertNoError();
    }

    std::vector<Inner> &objects() {
        return m_objects;
    }

    ~SSBO() {
        if (0 != m_ssboId) {
            glDeleteBuffers(1, &m_ssboId);
        }
    }
};

class ShaderLights
        : public Shader,
          public Observer<ViewMatrix>, public Observer<ProjectionMatrix> {
private:
    struct alignas(16) Light {
        glm::vec3 position;
        float _padding;
        glm::vec4 color;
        float intensity;
//        float _padding;
    };

    ShaderProgram program;
    ViewMatrix viewMatrix;
    ProjectionMatrix projectionMatrix;
    SSBO<Light> lights;
    glm::vec4 ambientColor = glm::vec4(0.1, 0.1, 0.1, 1.0);
    int32_t flags = 0; // Lightning features, see fragment/lights.glsl

    const int32_t FLAG_AMBIENT = 1 << 0;
    const int32_t FLAG_DIFFUSE = 1 << 1;
    const int32_t FLAG_SPECULAR = 1 << 2;
    const int32_t FLAG_HALFWAY = 1 << 3;

    static inline void assertNoError() {
#ifdef DEBUG_ASSERTIONS
        GLenum err = glGetError();
        DEBUG_ASSERT(GL_NO_ERROR == err);
#endif
    }

    explicit ShaderLights(ShaderProgram program) : program(std::move(program)),
                                                   viewMatrix(glm::mat4(1)),
                                                   projectionMatrix(glm::mat4(1)),
                                                   lights(0) {
        lights.objects().emplace_back(Light{
                .position = glm::vec3(0, 10, 0),
                ._padding = 0,
//                .color = glm::vec4(0.385, 0.647, 0.812, 1.0),
                .color = glm::vec4(1, 0, 0, 1.0),
                .intensity = 0.1,
        });

        lights.objects().emplace_back(Light{
                .position = glm::vec3(0, -10, 0),
                ._padding = 0,
//                .color = glm::vec4(0.385, 0.647, 0.812, 1.0),
                .color = glm::vec4(0, 1, 0, 1.0),
                .intensity = 0.1,
        });
        lights.send();
    }

public:
    ShaderLights(const ShaderLights &other) = delete;

    ShaderLights(ShaderLights &&other) noexcept:
            program(std::move(other.program)),
            viewMatrix(other.viewMatrix),
            projectionMatrix(other.projectionMatrix),
            lights(std::move(other.lights)) {
    }

    static std::optional<std::shared_ptr<ShaderLights>> load(const ShaderLoaderV2 &loader) {
        auto maybeVertexShader = loader.loadVertex("lights.glsl");
        if (!maybeVertexShader.has_value()) {
            return {};
        }
        auto maybeFragmentShader = loader.loadFragment("lights.glsl");
        if (!maybeFragmentShader.has_value()) {
            return {};
        }

        auto maybeShaderProgram = ShaderProgram::link(maybeFragmentShader.value(),
                                                      maybeVertexShader.value());
        if (!maybeShaderProgram.has_value()) {
            return {};
        }

        auto inner = ShaderLights(maybeShaderProgram.value());
        auto self = std::make_shared<ShaderLights>(std::move(inner));
        return std::move(self);
    }

#define BITFLAG(SET_FUNC_NAME, HAS_FUNC_NAME, FLAG_NAME) \
    void SET_FUNC_NAME(bool enabled) { \
        if (enabled) { \
            flags |= (FLAG_NAME); \
        } else { \
            flags &= ~(FLAG_NAME); \
        } \
    } \
    [[nodiscard]] inline bool HAS_FUNC_NAME() { \
        return FLAG_NAME; \
    } \

    BITFLAG(setAmbientEnabled, hasAmbient, FLAG_AMBIENT);

    BITFLAG(setDiffuseEnabled, hasDiffuse, FLAG_DIFFUSE);

    BITFLAG(setSpecularEnabled, hasSpecular, FLAG_SPECULAR);

    BITFLAG(setHalfwayEnabled, hasHalfway, FLAG_HALFWAY);

#undef BITFLAG

    void setAmbientColor(glm::vec3 color) {
        ambientColor = glm::vec4(color, 1);
    }

    void modelMatrix(glm::mat4 mat) {
        DEBUG_ASSERT(program.isBound());
        program.bindParam("modelMatrix", mat);
    }

    void cameraPosition(glm::vec3 cameraPosition) {
        DEBUG_ASSERT(program.isBound());
        program.bindParam("cameraPosition", cameraPosition);
    }

    void update(const ViewMatrix &action) override {
        viewMatrix = action;
    }

    void update(const ProjectionMatrix &action) override {
        projectionMatrix = action;
    }

    void bind() override {
        lights.bind();
        program.bind();
        // vertex uniforms
        program.bindParam("normalMatrix", glm::mat3(1));
        program.bindParam("viewMatrix", viewMatrix.viewMatrix);
        program.bindParam("projectionMatrix", projectionMatrix.projectionMatrix);

        // fragment uniforms
        program.bindParam("ambientColor", ambientColor);
        program.bindParam("flags", flags);
    }

    void unbind() override {
        program.unbind();
    }
};