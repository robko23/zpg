#pragma once

#include "Shader.h"
#include <algorithm>
#include <cstdint>
#define GLM_ENABLE_EXPERIMENTAL
#include "../LightGLSL.h"
#include "../LightsCollection.h"
#include "../Material.h"
#include "ShaderCommon.h"
#include <glm/gtx/string_cast.hpp>

class ShaderLights
    : public ShaderCommon<ShaderLights, "lights.glsl", "lights.glsl"> {
  private:
    std::shared_ptr<LightsCollection> lightCollection;
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

    void flagsUpdated() {
        this->bind();
        program.bindParam("flags", flags);
        this->unbind();
    }

  protected:
    void onCameraPositionChange(glm::vec3 cameraPosition) override {
        DEBUG_ASSERT(program.isBound());
        program.bindParam("cameraPosition", cameraPosition);
    }

  public:
    using ShaderCommon::ShaderCommon;

    ShaderLights(const ShaderLights &other) = delete;

    ShaderLights(ShaderLights &&other) noexcept
        : ShaderCommon(std::move(other)),
          lightCollection(std::move(other.lightCollection)) {}

#define BITFLAG(SET_FUNC_NAME, HAS_FUNC_NAME, FLAG_NAME)                       \
    void SET_FUNC_NAME(bool enabled) {                                         \
        if (enabled) {                                                         \
            flags |= (FLAG_NAME);                                              \
        } else {                                                               \
            flags &= ~(FLAG_NAME);                                             \
        }                                                                      \
        flagsUpdated();                                                        \
    }                                                                          \
    [[nodiscard]] inline bool HAS_FUNC_NAME() { return FLAG_NAME; }

    BITFLAG(setAmbientEnabled, hasAmbient, FLAG_AMBIENT);

    BITFLAG(setDiffuseEnabled, hasDiffuse, FLAG_DIFFUSE);

    BITFLAG(setSpecularEnabled, hasSpecular, FLAG_SPECULAR);

    BITFLAG(setHalfwayEnabled, hasHalfway, FLAG_HALFWAY);

#undef BITFLAG

    void setLightCollection(const std::shared_ptr<LightsCollection> val) {
        lightCollection = val;
    }

    void applyConstant() {
        setAmbientEnabled(true);
        setDiffuseEnabled(false);
        setSpecularEnabled(false);
        setHalfwayEnabled(false);
    }

    void applyLambert() {
        setAmbientEnabled(true);
        setDiffuseEnabled(true);
        setSpecularEnabled(false);
        setHalfwayEnabled(false);
    }

    void applyPhong() {
        setAmbientEnabled(true);
        setDiffuseEnabled(true);
        setSpecularEnabled(true);
        setHalfwayEnabled(false);
    }

    void applyBlinnPhong() {
        setAmbientEnabled(true);
        setDiffuseEnabled(true);
        setSpecularEnabled(true);
        setHalfwayEnabled(true);
    }

    void setMaterial(const Material &material) {
        program.bind();
        program.bindParam("material.ambient", material.getAmbient());
        program.bindParam("material.diffuse", material.getDiffuse());
        program.bindParam("material.specular", material.getSpecular());
        program.bindParam("material.shininess", material.getShininess());
        program.unbind();
    }

    void bind() override {
        ShaderCommon::bind();
        DEBUG_ASSERT_NOT_NULL(lightCollection);
        lightCollection->bind(0);
    }
};
