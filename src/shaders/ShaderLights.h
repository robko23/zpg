#pragma once

#include "Shader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "SSBO.h"
#include "ShaderCommon.h"

/*
 * Matching declaration for struct Light in fragment/lights.glsl
 */
class alignas(16) LightGLSL final {
private:
    /*
     * More info about memory layout here:
     * https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout
     */
    glm::vec3 position;
    uint32_t _padding_0 = 0;

    glm::vec3 direction;
    uint32_t _padding_1 = 0;

    glm::vec3 attenuation;
    uint32_t _padding_2 = 0;

    glm::vec4 color;

    int type = 1; // 0 - none, 1 - point, 2 - direction, 3 - reflector
    float cutoff = 0;
public:
    explicit LightGLSL(const glm::vec3 &position, const glm::vec3 &direction,
                       const glm::vec3 &attenuation,
                       const glm::vec4 &color) : position(position), direction(direction),
                                             attenuation(attenuation), color(color) {}

    [[nodiscard]] const glm::vec3 &getPosition() const {
        return position;
    }

    void setPosition(const glm::vec3 &position) {
        LightGLSL::position = position;
    }

    [[nodiscard]] const glm::vec3 &getDirection() const {
        return direction;
    }

    void setDirection(const glm::vec3 &direction) {
        LightGLSL::direction = direction;
    }

    [[nodiscard]] const glm::vec3 &getAttenuation() const {
        return attenuation;
    }

    void setAttenuation(const glm::vec3 &attenuation) {
        LightGLSL::attenuation = attenuation;
    }

    [[nodiscard]] const glm::vec4 &getColor() const {
        return color;
    }

    void setColor(const glm::vec4 &color) {
        LightGLSL::color = color;
    }
};
// So that I don't accidentally add more fields
static_assert(sizeof(LightGLSL) == 80);

class Material {
private:
    friend class ShaderLights;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;

public:
    Material(const glm::vec4 &ambient, const glm::vec4 &diffuse, const glm::vec4 &specular,
             float shininess) : ambient(ambient), diffuse(diffuse), specular(specular),
                                shininess(shininess) {}

    const glm::vec4 &getAmbient() const {
        return ambient;
    }

    void setAmbient(const glm::vec4 &ambient) {
        Material::ambient = ambient;
    }

    const glm::vec4 &getDiffuse() const {
        return diffuse;
    }

    void setDiffuse(const glm::vec4 &diffuse) {
        Material::diffuse = diffuse;
    }

    const glm::vec4 &getSpecular() const {
        return specular;
    }

    void setSpecular(const glm::vec4 &specular) {
        Material::specular = specular;
    }

    float getShininess() const {
        return shininess;
    }

    void setShininess(float shininess) {
        Material::shininess = shininess;
    }
};

class ShaderLights
        : public ShaderCommon<ShaderLights, "lights.glsl", "lights.glsl"> {
private:
    SSBO<LightGLSL> lights;
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

    ShaderLights(ShaderLights &&other) noexcept:
            ShaderCommon(std::move(other)),
            lights(std::move(other.lights)) {
    }

    /*
     * Adds a light to the shader and sends it to the SSBO
     * @returns Index where the light is located. You can use it to modify it
     */
    size_t addLight(LightGLSL light) {
        size_t idx = lights.objects().size();
        lights.objects().emplace_back(std::move(light));
        lights.realloc();
        return idx;
    }

    LightGLSL &getLight(size_t idx) {
        DEBUG_ASSERT(idx < lights.objects().size());
        return lights.objects().at(idx);
    }

    void updateLight(size_t idx) {
        DEBUG_ASSERT(idx < lights.objects().size());
        lights.updateAt(idx);
    }

#define BITFLAG(SET_FUNC_NAME, HAS_FUNC_NAME, FLAG_NAME) \
    void SET_FUNC_NAME(bool enabled) { \
        if (enabled) { \
            flags |= (FLAG_NAME); \
        } else { \
            flags &= ~(FLAG_NAME); \
        }                                                \
        flagsUpdated();                                                \
    } \
    [[nodiscard]] inline bool HAS_FUNC_NAME() { \
        return FLAG_NAME; \
    } \


    BITFLAG(setAmbientEnabled, hasAmbient, FLAG_AMBIENT);

    BITFLAG(setDiffuseEnabled, hasDiffuse, FLAG_DIFFUSE);

    BITFLAG(setSpecularEnabled, hasSpecular, FLAG_SPECULAR);

    BITFLAG(setHalfwayEnabled, hasHalfway, FLAG_HALFWAY);

#undef BITFLAG

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
        program.bindParam("material.ambient", material.ambient);
        program.bindParam("material.diffuse", material.diffuse);
        program.bindParam("material.specular", material.specular);
        program.bindParam("material.shininess", material.shininess);
        program.unbind();
    }


    void bind() override {
        ShaderCommon::bind();
        lights.bind(0);
    }
};
