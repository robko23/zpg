#pragma once
#include "assertions.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"

enum LightType { None = 0, Point = 1, Directional = 2, Reflector = 3 };

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
    uint32_t id = UINT32_MAX;

    inline void assertInitialized() const {
        DEBUG_ASSERTF(id != UINT32_MAX, "This light is not initialized");
    }

  public:
    explicit LightGLSL(const glm::vec3 &position, const glm::vec3 &direction,
                       const glm::vec3 &attenuation, const glm::vec4 &color)
        : position(position), direction(direction), attenuation(attenuation),
          color(color) {}

    [[nodiscard]] const glm::vec3 &getPosition() const { return position; }

    void setId(uint32_t newId) {
        DEBUG_ASSERTF(
            id == UINT32_MAX,
            "Trying to reassign id to this light. Prev id: %u, new id: %u", id,
            newId);
        id = newId;
    }

    void setPosition(const glm::vec3 &position) {
        assertInitialized();
        LightGLSL::position = position;
    }

    void setDirection(const glm::vec3 &direction) {
        assertInitialized();
        LightGLSL::direction = direction;
    }

    void setAttenuation(const glm::vec3 &attenuation) {
        assertInitialized();
        LightGLSL::attenuation = attenuation;
    }

    void setColor(const glm::vec4 &color) {
        assertInitialized();
        LightGLSL::color = color;
    }

    void setType(LightType val) {
        assertInitialized();
        type = val;
    }

    void setCutoff(float val) {
        assertInitialized();
        cutoff = val;
    }

    [[nodiscard]] const glm::vec4 &getColor() const { return color; }
    [[nodiscard]] const glm::vec3 &getDirection() const { return direction; }
    [[nodiscard]] const glm::vec3 &getAttenuation() const {
        return attenuation;
    }
    [[nodiscard]] const uint32_t &getId() const {
        assertInitialized();
        return id;
    }

    [[nodiscard]] const LightType getType() const {
        return static_cast<LightType>(type);
    }
};
// So that I don't accidentally add more fields
static_assert(sizeof(LightGLSL) == 80);
