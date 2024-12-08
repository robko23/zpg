#pragma once

#include "glm/ext/vector_float4.hpp"
class Material {
  private:
    friend class ShaderLights;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;

  public:
    Material(const glm::vec4 &ambient, const glm::vec4 &diffuse,
             const glm::vec4 &specular, float shininess)
        : ambient(ambient), diffuse(diffuse), specular(specular),
          shininess(shininess) {}

    [[nodiscard]] const glm::vec4 &getAmbient() const { return ambient; }

    void setAmbient(const glm::vec4 &ambient) { Material::ambient = ambient; }

    [[nodiscard]] const glm::vec4 &getDiffuse() const { return diffuse; }

    void setDiffuse(const glm::vec4 &diffuse) { Material::diffuse = diffuse; }

    [[nodiscard]] const glm::vec4 &getSpecular() const { return specular; }

    void setSpecular(const glm::vec4 &specular) {
        Material::specular = specular;
    }

    [[nodiscard]] float getShininess() const { return shininess; }

    void setShininess(float shininess) { Material::shininess = shininess; }
};
